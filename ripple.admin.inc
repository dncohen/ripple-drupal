<?php

/**
 * Drupal form callback for general settings.
 */
function ripple_admin_settings_form($form, &$form_state) {
  $form['account'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ripple account'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['account'][RIPPLE_VAR_ACCOUNT] = array(
    '#type' => 'textfield',
    '#title' => t('Default account'),
    '#description' => t('Ripple wallet to use by default.'),
    '#default_value' => variable_get(RIPPLE_VAR_ACCOUNT, NULL),
  );

  $form['account'][RIPPLE_VAR_CURRENCY] = array(
    '#type' => 'textfield',
    '#title' => t('Preferred currency'),
    '#description' => t('Ripple currency to use by default.'),
    '#default_value' => variable_get(RIPPLE_VAR_CURRENCY, NULL),
  );



  $form['rest'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ripple REST server'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['rest'][RIPPLE_VAR_REST_URL] = array(
    '#type' => 'textfield',
    '#title' => t('Ripple REST API server'),
    '#description' => t('Defaults to <em>http://localhost:5990</em>.'),
    '#default_value' => variable_get(RIPPLE_VAR_REST_URL, NULL),
  );

  $form['rest'][RIPPLE_VAR_REST_VERSION] = array(
    '#type' => 'textfield',
    '#size' => 4,
    '#title' => t('Ripple REST API version'),
    '#description' => t('Defaults to <em>v1</em>.'),
    '#default_value' => variable_get(RIPPLE_VAR_REST_VERSION, NULL),
  );



  $form = system_settings_form($form);
  $form['#submit'][] = 'ripple_admin_unset_variables';
  return $form;
}

/**
 * Drupal form submit callback.
 *
 * Unset empty variables.  This way they will get the default values passed to variable_get.
 */
function ripple_admin_unset_variables($form, &$form_state) {
  foreach ($form_state['values'] as $key => $val) {
    if (!$val) {
      variable_del($key);
    }
  }
}