const char* MainPageHtml1 = R"-----(
<!DOCTYPE html>

<html>
<head>
    <meta charset="utf-8" />
    <title>EagleDecorations Main</title>
</head>
<body>
    <h1>EagleDecorations Controller</h1>

    <h2>Status</h2>
)-----";

const char* MainPageHtml2 = R"-----(

    <h1>Wireless Provisioning</h1>
<p>
    Chose a network from the list or enter one directly.
</p>
<h2>Visible Networks</h2>
<form>
    <select name="ssid">
)-----";

const char* MainPageHtml3 = R"-----(
    </select>
    &nbsp;&nbsp; Password: <input type="text" name="password"/>
    <input type="submit" value="Register"/>
</form>
<h3>Other Network</h3>
<form>
    SSID: <input type="text" name="ssid" />
    &nbsp;&nbsp; Password: <input type="text" name="password"/>
    <input type="submit" value="Register"/>
</form>

<h1>Control</h1>
)-----";

const char* MainPageHtml4 = R"-----(

<a href="http://www.riderx.info/provisioning-and-using-the-esp8266-controller/">Command Documentation</a>

<h2>Direct colors (single) - Direct colors (series)</h2>

<p><b>Note</b> that bright colors may overheat your esp controller or not be full brightness if you are powering through USB. Single colors will
set a single color; series will add the color to the current animation series.</p>
)-----";

const char* MainPageHtml5 = R"-----(

<h2>Examples</h2>

<h3>Blend to violet color over one second</h3>
<a href="/message?r=1&content=rgbx180,0,180,1000">rgbx180,0,180,1000</a>

<h3>Yellow blue alternate</h3>
<a href="/message?r=1&content=alt,150,150,000,000,000,150,250">alt,150,150,000,000,000,150,250</a>

<h3>Individual addressing red green blue, 11 leds each</h3>
<a href="/message?r=1&content=indx011,000044440000004400">indx011,000044440000004400</a>

<h3>Flash and fade</h3>
<a href="/message?r=1&content=fdcx250,10,500">fdcx250,10,500</a>

<h3>Color rotate</h3>
<a href="/message?r=1&content=colx5000,180">colx5000,180</a>

<h3>Save current color/animation</h3> 
<a href="/message?r=1&content=s">s</a>

<h3>Change LED count to 10 (will reboot!)</h3>
<a href="/message?r=1&content=nx10">nx10</a>

<h2>Custom command (single)</h2>
<form action="/message">
    <input type="hidden" name="r" value="1"/>
    Content: <input type="text" name="content" />&nbsp;&nbsp;
    <input type="submit" value="Execute"/>
</form>

<h2>Custom command (series)</h2>
<form action="/message">
    <input type="hidden" name="r" value="1"/>
    Content: <input type="text" name="content" />&nbsp;&nbsp;    
    Period: <input type="text" name="period" />&nbsp;&nbsp;
    <input type="submit" value="Execute"/>
</form>
<h2>&nbsp;</h2>

</body>
</html>
)-----";
