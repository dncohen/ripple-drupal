<?php

/**
 * This helper queries the ripple rest server for payments to or from an
 * account.  Performs one iteration, returning as many as 20 past
 * payments.  May be called from cron or batch jobs.
 *
 * "Receive" is a misnomer as this function finds both sent and received payments for the account.
 */
function ripple_rest_receive_payments($account) {
  $lock_key = 'ripple_' . $account;
  $lock = lock_acquire($lock_key);
  if (!$lock) {
    $payments = FALSE;
  }
  else {
    $ledger_key = 'ripple_' . $account . '_next_ledger';
    $next_ledger = variable_get($ledger_key, 1);
    $page_key = 'ripple_' . $account . '_next_page';
    $next_page = variable_get($page_key, 1);

    $result = ripple_rest('accounts/' . $account . '/payments', array(
                            'query' => array(
                              //'directions' => 'incoming', <== commented out because we want both.
                              'earliest_first' => 'true',
                              'start_ledger' => $next_ledger,
                              'page' => $next_page,
                              'results_per_page' => 20,
                            )));

    if (module_exists('devel') && user_access('access devel information')) {
      // Sanity check.  Make sure page option works as documented.
      // note: Looks like it doesn't work!
      $result2 = ripple_rest('accounts/' . $account . '/payments', array(
                               'query' => array(
                                 //'directions' => 'incoming', <== we want both.
                                 'earliest_first' => 'true',
                                 'start_ledger' => $next_ledger,
                                 'page' => $next_page + 1,
                                 'results_per_page' => 20,
                               )));
      dpm($result, __FUNCTION__ . " page $next_page");
      dpm($result2, __FUNCTION__ . " page $next_page + 1");
    }

    if (!$result['success']) {
      $payments = FALSE;
    }
    else {
      $payments = $result['payments'];
      foreach ($result['payments'] as $payment) {
        module_invoke_all('ripple_payment', $payment);
        $last_ledger = max($last_ledger, $payment['ledger']);
      }

      if (FALSE) { // disabled while testing.
        // Update variables so we don't repeat work the next time we are called.
        if (count($result['payments']) >= 20) {
          // If 20 payments returned, we need to inspect next page, next ledger.
          variable_set($page_var, $next_page + 1);
        }
        else {
          // Otherwise, advance ledger.
          variable_set($ledger_var, $last_ledger + 1);
          variable_set($page_var, 1);
        }
      }

    }
    lock_release($lock_key);
  }
  return $payments;
}

function ripple_rest_batch_receive_payments($account, &$context) {
  // @todo make this function more intelligent, so that ripple_rest_receive_payments is called more than once when it needs to be.
  ripple_rest_receive_payments($account);
    // @todo if more pending payments, set $finished < 1.

}

function ripple_rest_batch_get_server_status($options_in, &$context) {
  $sandbox = &$context['sandbox'];
  $results = &$context['results'];
  $finished = &$context['finished'];

  $server = ripple_rest('server');
  dpm($server, __FUNCTION__);
  dpm(func_get_args(), __FUNCTION__);
  $results['ripple_rest_server'] = $server;
  $context['results']['test'] = $server;
}

function ripple_rest_batch_find_payments_by_destination_tag($options_in, &$context) {
  dpm(func_get_args(), __FUNCTION__);
  debugger();
  $sandbox = &$context['sandbox'];
  $results = &$context['results'];
  $finished = &$context['finished'];

  if (!isset($sandbox[__FUNCTION__])) {
    $sandbox[__FUNCTION__] = $options_in + array(
      // Defaults
      'directions' => 'incoming',
      'start_ledger' => 0,
    );
    $sandbox[__FUNCTION__]['page'] = 0;

  }
  $options = &$sandbox[__FUNCTION__];

  $payments = ripple_rest('accounts/' . $options['account'] . '/payments', array(
                            'query' => array(
                              'directions' => $options['directions'],
                              'earliest_first' => TRUE,
                              'start_ledger' => $options['start_ledger'],
                              'page' => $options['page'],
                              'results_per_page' => 20,
                            ),
                          ));
  dpm($payments, __FUNCTION__);
  if ($payments['success']) {
    foreach ($payments['payments'] as $payment) {
      if ($payment['destination_tag'] == $options['destination_tag']) {
        $results['payments'][] = $payment;
      }
    }
  }
  else {
    $message = 'Unexpected error checking payments to %account with destination tag %tag (%msg).';
    $args = array(
      '%account' => $options['account'],
      '%tag' => $options['destination_tag'],
      '%msg' => $payments['message'],
    );
    drupal_set_message(t($message, $args), 'error');
    watchdog('ripple', $message, $args, WATCHDOG_ERROR);
    $results[__FUNCTION__] = $payments;
  }
}