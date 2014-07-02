A quick way to get familiar with this module is to add it to a Commerce Kickstart site.

First download and install Commerce Kickstart.  See https://www.drupal.org/project/commerce_kickstart

Find the sites/all/modules directory of your Commerce Kickstart installation.  There, install the ripple module. See https://www.drupal.org/project/ripple

While logged into your Commerce Kickstart as administrator, visit admin/modules.  For example, https://example.com/admin/modules, if your home URL is https://example.com.  Enable the Ripple and Ripple Commerce modules.

Visit the Store settings >> Payment methods page at admin/commerce/config/payment-methods.  Notice the Ripple via REST API payment method provided by ripple_commerce.module.  Click the "enable" link for this payment method.

You will now see Ripple via REST API under the enabled payment methods.  Click "edit" on that row, and on the subsequent page, under Actions, click the edit button.  Here, specify your Ripple account, where payments to your store will be received.  You may need to specify the URL of your Ripple REST server, if not running on localhost.

Optionally, if you wish to accept payment in XRP, navigate to admin/commerce/config/currency, and enable the XRP currency.  Also make XRP the store's default currency (otherwise it will attempt to convert to USD on checkout).  The Commerce Kickstart default currency is USD.  For USD payments to succeed, be sure your ripple wallet trusts a USD issuer.

Commerce Kickstart includes shipping fees, which increase the dollar amounts when testing, and cause even more problems when using currencies other than USD.  Rather than reconfigure all the shipping rules, we'll work around these issues by enabling free shipping on our test orders.  A kludgy but fast way to do this is: First, navigate to Store Settings >> Shipping.  Look for "Free Shipping" and click "configure component" on that row.  Look for the "NOT Data comparison" and click Delete.  By deleting this logic, we are enabling free shipping for all our testing.

Now we'll choose a product to test with.  On the sites front page, near the bottom, are some product links.  The water bottle is a convenient choice.  When we click that image we are taken to the path drinks/guy-h20.  Because we are logged in as an administrator, we can change the product's price.  While viewing the product click the Edit tab link.  On the edit form, under Product Variations, click edit again to change the price.  For testing, you may lower the price to say 1 XRP.  Click Save.

One the product view page, click "Add to cart".

At the top of any page, click "Checkout".  The checkout process is several pages long but straightforward to navigate.  Enter fake or real shipping information, and be sure to select the free shipping option.

Be sure to select the "Ripple via REST API" payment method.  When you continue, you will be prompted to log into your Ripple wallet.

After you've paid for the produce, return to the Commerce Kickstart admin pages to confirm the order status.

Note that Commerce Kickstart is not notified immediately of any ripple ledger changes.  It must learn about the payment by inspecting the ledger.  To force this to happen immediately, you may manually run Drupal's "cron".  Navigate to /admin/reports/status.  Then click the "Run cron manually" link.  If you don't do this, order status will be updated eventually.  For testing it is nice to have it happen right away.

Next navigate to the orders page at admin/commerce/orders.  If all is working, the test transaction will be complete.

