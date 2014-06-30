A quick way to get familiar with this module is to add it to a Commerce Kickstart site.

First download and install Commerce Kickstart.  See https://www.drupal.org/project/commerce_kickstart

Find the sites/all/modules directory of your Commerce Kickstart installation.  There, install the ripple module. See https://www.drupal.org/project/ripple

While logged into your Commerce Kickstart as administrator, visit admin/modules.  For example, https://example.com/admin/modules, if your home URL is https://example.com.  Enable the Ripple and Ripple Commerce modules.

Visit the Store settings >> Payment methods page at admin/commerce/config/payment-methods.  Notice the Ripple via REST API payment method provided by ripple_commerce.module.  Click the "enable" link for this payment method.

You will now see Ripple via REST API under the enabled payment methods.  Click "edit" on that row, and on the subsequent page, under Actions, click the edit button.  Here, specify your Ripple account, where payments to your store will be received.  You may need to specify the URL of your Ripple REST server, if not running on localhost.

Optionally, if you wish to accept payment in XRP, navigate to admin/commerce/config/currency, and enable the XRP currency.  The Commerce Kickstart default currency is USD.  For USD payments to succeed, be sure your ripple wallet trusts a USD issuer.

Visit the product variation management page at admin/commerce/products/variations.  For testing, you may want to edit a product, change its price to a small value, and optionally change the currency.
