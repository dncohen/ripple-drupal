<?php

function ripple_commerce_rest_settings_form($settings = array()) {

  // Defaults to avoid PHP notices.
  $settings += array(
    RIPPLE_VAR_ACCOUNT => NULL,
    RIPPLE_VAR_CURRENCY => NULL,
    RIPPLE_VAR_REST_URL => NULL,
    RIPPLE_VAR_REST_VERSION => NULL,
  );

  $form = array();

  $form[RIPPLE_VAR_ACCOUNT] = array(
    '#type' => 'textfield',
    '#title' => t('Ripple account'),
    '#description' => t('TODO'),
    '#default_value' => $settings[RIPPLE_VAR_ACCOUNT],
  );

  $form[RIPPLE_VAR_CURRENCY] = array(
    '#type' => 'textfield',
    '#title' => t('Default currency'),
    '#description' => t('TODO'),
    '#default_value' => $settings[RIPPLE_VAR_CURRENCY],
    '#size' => 3,
  );

  $form[RIPPLE_VAR_REST_URL] = array(
    '#type' => 'textfield',
    '#title' => t('Ripple server REST URL'),
    '#description' => t('TODO'),
    '#default_value' => $settings[RIPPLE_VAR_REST_URL],
  );

  $form[RIPPLE_VAR_REST_VERSION] = array(
    '#type' => 'textfield',
    '#title' => t('Ripple server REST URL'),
    '#description' => t('TODO'),
    '#default_value' => $settings[RIPPLE_VAR_REST_VERSION],
  );

  return $form;
}

function ripple_commerce_rest_submit_formXXX($payment_method, $pane_values, $checkout_pane, $order) {
  dpm(func_get_args(), __FUNCTION__);
}


/**
 * Payment method callback: checkout form submission.
 *
 * Processes payment as necessary using data inputted via the payment details
 * form elements on the form, resulting in the creation of a payment
 * transaction.
 *
 * @param array $payment_method
 *   An array containing info hook values and user settings.
 */
function ripple_commerce_rest_submit_form_submitXXX($payment_method, $pane_form, $pane_values, $order, $charge) {
  dpm(func_get_args(), __FUNCTION__);

  // Get order to wrapper.
  $wrapper = entity_metadata_wrapper('commerce_order', $order);

  // Get order price.
  $amount = $wrapper->commerce_order_total->amount->value();

  // @link https://drupal.org/node/2136989 $0 invoice throws an error @endlink
  if (empty($amount)) {
    watchdog('ripple_commerce', 'Skipping payment on order @id for zero balance.', array('@id' => $order->order_id), WATCHDOG_INFO, l(t('view order'), 'admin/commerce/orders/' . $order->order_id));
    commerce_checkout_complete($order);
    drupal_goto(_commerce_bitpay_redirecturl($payment_method, $order));
  }

  $order->data['ripple_commerce_rest'] = $pane_values;

  ripple_commerce_transaction_create($payment_method, $order, $charge);
}



/**
 * Payment method callback: redirect form.
 *
 * How to redirect is copied from commerce_paypal_ec_redirect_form.
 */
function ripple_commerce_rest_redirect_form($form, &$form_state, $order, $payment_method) {

  // Payment methods defaults.
  foreach (array(
             RIPPLE_VAR_REST_URL => 'http://localhost:5990',
             RIPPLE_VAR_REST_VERSION => 'v1',
             RIPPLE_VAR_ACCOUNT => NULL,
             RIPPLE_VAR_CURRENCY => NULL,
           ) as $key => $default) {
    if (empty($payment_method['settings'][$key])) {
      $payment_method['settings'][$key] = variable_get($key, $default);
      if (empty($payment_method['settings'][$key])) {
        // Show an error message and go back a page.
        drupal_set_message(t('Ripple is not configured properly to accept payments.  No value for %key.', array('%key' => $key)), 'error');
        commerce_payment_redirect_pane_previous_page($order, t('Redirect to Ripple wallet failed.'));
      }
    }
  }

  $payment_method['settings'] += array(
    'server' => variable_get(RIPPLE_VAR_REST_URL, 'http://localhost:5990'),
    'version' => variable_get(RIPPLE_VAR_REST_VERSION, 'v1'),
  );

  // We need a transaction, in addition to an order, to process payment.
  $transaction = ripple_commerce_transaction_create($payment_method, $order);

  // Return and cancel URLs copied from commerce_paypal_ec_set_express_checkout().
  $return_url = url('checkout/' . $order->order_id . '/payment/return/' . $order->data['payment_redirect_key'], array('abosolute' => TRUE));
  $abort_url = url('checkout/' . $order->order_id . '/payment/back/' . $order->data['payment_redirect_key'], array('abosolute' => TRUE));

  // Calculate proper amount.
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $orig_amount = $order_wrapper->commerce_order_total->amount->value();
  $currency = $order_wrapper->commerce_order_total->currency_code->value();

  // Build the ripple URI.  https://ripple.com/wiki/Ripple_URIs
  $amount = commerce_currency_amount_to_decimal($orig_amount, $currency) . '/' . $currency;
  $query = array(
    //'dt' => $order->order_number,
    'dt' => $transaction->transaction_id,
    'to' => $payment_method['settings'][RIPPLE_VAR_ACCOUNT],
    'amount' => $amount,
    'label' => t('Payment for !site_name order #!order_number', array(
                   '!site_name' => variable_get('site_name', 'Drupal'),
                   '!order_number' => $order->order_number,
                 )),

    // Current ripple client seems to do nothing with these urls.
    'return_url' => $return_url,
    'abort_url' => $abort_url,
  );

  $url = url('https://ripple.com//send', array(
               'query' => $query,
             ));

  // Update the order status.
  $order->data['ripple_account'] = $payment_method['settings'][RIPPLE_VAR_ACCOUNT];
  $order->data['payment_method_settings'] = $payment_method['settings']; // not sure needed.
  commerce_order_status_update($order, 'checkout_payment', FALSE, NULL, t('Customer redirected to Ripple wallet.'));
  commerce_order_save($order);

  drupal_goto($url);

}
