const char* MainPageStartHtml = R"-----(
<!DOCTYPE html>

<html>
<head>
    <meta charset="utf-8" />
    <title>EagleDecorations Main</title>
</head>
<body>
    <h1>EagleDecorations Wireless Provisioning</h1>
<p>
    Chose a network from the list or enter one directly.
</p>
<h2>Visible Networks</h2>
<form>
    <select name="ssid">
)-----";

const char* MainPageEndHtml = R"-----(
    </select>
    &nbsp;&nbsp; Password: <input type="text" name="password"/>
    <input type="submit" value="submit"/>
</form>
<h3>Other Network</h3>
<form>
    SSID: <input type="text" name="ssid" />
    &nbsp;&nbsp; Password: <input type="text" name="password"/>
    <input type="submit" value="submit"/>
</form>

<h1>Control</h1>

<h2>Examples</h2>

<h3>Blend to violet color over one second</h3>
<a href="/message?r=1&content=rgbx180,0,180,1000">/message?content=rgbx180,0,180,1000</a>

<h3>Yellow blue alternate</h3>
<a href="/message?r=1&content=alt,150,150,000,000,000,150,250">/message?content=alt,150,150,000,000,000,150,250</a>

<h3>Individual addressing red green blue, 11 leds each</h3>
<a href="/message?r=1&content=indx011,000044440000004400">/message?content=indx011,000044440000004400</a>

<h3>Flash and fade</h3>
<a href="/message?r=1&content=fdcx250,10,500">/message?content=fdcx250,10,500</a>

<h3>Color rotate</h3>
<a href="/message?r=1&content=colx5000,180">/message?content=colx5000,180</a>

<h3>Save current color/animation</h3> 
<a href="/message?r=1&content=s">/message?content=s</a>

</body>
</html>
)-----";
