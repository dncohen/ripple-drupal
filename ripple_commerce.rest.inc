<?php

function ripple_commerce_rest_settings_form($settings = array()) {
  dpm(func_get_args(), __FUNCTION__);

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

function ripple_commerce_rest_submit_form($payment_method, $pane_values, $checkout_pane, $order) {
  dpm(func_get_args(), __FUNCTION__);
}

/**
 * Payment method callback: redirect form.
 *
 * How to redirect is copied from commerce_paypal_ec_redirect_form.
 */
function ripple_commerce_rest_redirect_form($form, &$form_state, $order, $payment_method) {
  dpm(func_get_args(), __FUNCTION__);
  dpm($order, __FUNCTION__);

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

  // Calculate proper amount, using complicated commerce APIs.
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $orig_amount = $order_wrapper->commerce_order_total->amount->value();
  $currency = $order_wrapper->commerce_order_total->currency_code->value();
  $amount = commerce_currency_amount_to_decimal($orig_amount, $currency) . '/' . $currency;

  // Return and cancel URLs copied from commerce_paypal_ec_set_express_checkout().
  $return_url = url('checkout/' . $order->order_id . '/payment/return/' . $order->data['payment_redirect_key'], array('abosolute' => TRUE));
  $abort_url = url('checkout/' . $order->order_id . '/payment/back/' . $order->data['payment_redirect_key'], array('abosolute' => TRUE));

  // Build the ripple URI.  https://ripple.com/wiki/Ripple_URIs
  $query = array(
    'dt' => $order->order_number,
    'to' => $payment_method['settings'][RIPPLE_VAR_ACCOUNT],
    'amount' => $amount,
    'label' => t('Processing !site_name order #!order_number', array(
                   '!site_name' => variable_get('site_name', 'Drupal'),
                   '!order_number' => $order->order_number,
                 )),
    'return_url' => $return_url,
    'abort_url' => $abort_url,
  );

  $url = url('https://ripple.com//send', array(
               'query' => $query,
             ));

  drupal_goto($url);

}
