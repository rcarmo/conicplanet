<?php

/******************************************************************************
    Conic Planet - CGI program to render an image of a planet
    Copyright (C) 2000 Mukund Iyer <muks@crosswinds.net>
    
    Most portions of this code were developed by and are
    Copyright (C) 2000 Hari Nair <hari@alumni.caltech.edu>
    for XPlanet 0.72

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
******************************************************************************/

$cgiapp = "http://conic.sourceforge.net/cgi-bin/conicplanet.cgi";

$img = $cgiapp;
$prepend = "?";

reset($HTTP_GET_VARS);
while (list($key, $val) = each($HTTP_GET_VARS))
{
	$img = $img . $prepend . urlencode($key) . "=" . urlencode($val);
	$prepend = "&";
}

?>

<html>
<head>
<title>Conic Planet Demo</title>
</head>

<body bgcolor="#FFFFFF" link="#008080" vlink="#008080" alink="#000000">

<p align="center"><font size="5" face="Arial"><strong>Conic
Planet Demo</strong></font></p>

<p align="center"><img src="<?php print($img); ?>" border="0" alt="Conic Planet generated image"></p>

<p align="center">&nbsp;</p>

<p align="center"><font size="3" face="Arial">If you would like to
routinely rip this image for your wallpaper with a script,<br>
you may do so. But please set your interval to over 2 hours as
the image generation<br>
is computationally time-consuming, and there are other people who
use this too :-)</font></p>

<p align="center"><font face="Arial">The URL for the above JPEG image is:<br>
<a href="<?php print($img); ?>"><?php print($img); ?></a></font></p>

<p align="center">&nbsp;</p>

<form action="http://conic.sourceforge.net/conicplanet/test.php" method="GET">
    <p align="center"><font face="Arial"><strong>Enter your own
    options below:<br></strong></font>
    <font size="2" face="Arial"><strong>(more
    options can be found in README in the source distribution)</strong></font></p>
    <div align="center"><center><table border="1" cellpadding="5"
    cellspacing="0" width="75%">
        <tr>
            <td align="right" width="50%"><font face="Arial">Select
            planetary body</font></td>
            <td width="50%"><select name="body" size="1">
                <option selected value="earth">Earth</option>
                <option value="mercury">Mercury</option>
                <option value="venus">Venus</option>
                <option value="moon">Moon</option>
                <option value="mars">Mars</option>
                <option value="jupiter">Jupiter</option>
                <option value="io">Io</option>
                <option value="europa">Europa</option>
                <option value="ganymede">Ganymede</option>
                <option value="callisto">Callisto</option>
                <option value="saturn">Saturn</option>
                <option value="titan">Titan</option>
                <option value="uranus">Uranus</option>
                <option value="neptune">Neptune</option>
                <option value="pluto">Pluto</option>
            </select></td>
        </tr>
        <tr>
            <td align="right" width="50%"><font face="Arial">Projection
            Type</font></td>
            <td width="50%"><select name="projection" size="1">
                <option selected value="orthographic">Orthographic</option>
                <option value="rectangular">Rectangular</option>
                <option value="mollweide">Mollweide</option>
                <option value="mercator">Mercator</option>
                <option value="azimuthal">Azimuthal</option>
            </select></td>
        </tr>
        <tr>
            <td align="right" width="50%"><font face="Arial">Show
            current cloud covering?</font></td>
            <td width="50%"><input type="radio" name="clouds"
            value="1"><font face="Arial"> Yes </font><input
            type="radio" checked name="clouds" value="0"><font
            face="Arial"> No</font></td>
        </tr>
        <tr>
            <td align="right" width="50%"><font face="Arial">Use
            night map?</font></td>
            <td width="50%"><input type="radio" checked
            name="night" value="1"><font face="Arial"> Yes </font><input
            type="radio" name="night" value="0"><font
            face="Arial"> No</font></td>
        </tr>
        <tr>
            <td align="right" width="50%"><font face="Arial">Rotate
            angle</font></td>
            <td width="50%"><input type="text" size="5"
            name="rotate" value="0"></td>
        </tr>
        <tr>
            <td align="right" width="50%"><font face="Arial">Use
            grid?</font></td>
            <td width="50%"><input type="radio" name="grid"
            value="1"><font face="Arial"> Yes </font><input
            type="radio" checked name="grid" value="0"><font
            face="Arial"> No</font></td>
        </tr>
        <tr>
            <td align="right" width="50%"><font face="Arial">Perform
            filtering?</font></td>
            <td width="50%"><input type="radio" name="filter"
            value="1"><font face="Arial"> Yes </font><input
            type="radio" checked name="filter" value="0"><font
            face="Arial"> No</font></td>
        </tr>
        <tr>
            <td align="right" width="50%"><font face="Arial">Position</font></td>
            <td width="50%"><select name="position" size="1">
                <option value="fixed">Fixed</option>
                <option value="dayside">Dayside</option>
                <option value="earthside">Earthside</option>
                <option value="moonside">Moonside</option>
                <option value="nightside">Nightside</option>
            </select></td>
        </tr>
        <tr>
            <td align="right" width="50%"><font face="Arial">Width
            of output image</font></td>
            <td width="50%"><input type="text" size="10"
            name="width" value="600"></td>
        </tr>
        <tr>
            <td align="right" width="50%"><font face="Arial">Height
            of output image</font></td>
            <td width="50%"><input type="text" size="10"
            name="height" value="600"></td>
        </tr>
    </table>
    </center></div><p align="center"><center><input type="submit"></center></p>
</form>

<p align="center">&nbsp;</p>

<p align="center"><font size="3" face="Arial">Thank you for using
Conic Planet!</font></p>

<p align="center"><font size="3" face="Arial">Visit the Conic
Planet project page at<br>
<a href="http://sourceforge.net/projects/conic/">http://sourceforge.net/projects/conic/</a></font></p>

<p align="center"><img src="http://sourceforge.net/sflogo.php?group_id=1054&type=1" alt="SourceForge" border="0" width="88" height="31"></p>
</body>
</html>
