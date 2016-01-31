<ol>
<li>Compiled using opencv 1.1pre1</li>
<li>Usage: avi-read video-file-name.avi > output.ply</li>
<li>Usage with optional parameters: avi-read video-file-name.avi <Erode Factor> <Camera Angle> <Calibration Frame> > output.ply</li>
<li>sample.ply is a sample output of this program which can be viewed/loaded with <a href='http://meshlab.sourceforge.net'>http://meshlab.sourceforge.net</a></li>
<li>the turn-table I was using was rotating too fast and there is some missing information</li>
<li>There is still a problem with the math in the current source code (the shape is distorted)</li>
<li>inspired from <a href='http://www.instructables.com/id/3-D-Laser-Scanner/'>http://www.instructables.com/id/3-D-Laser-Scanner/</a></li>
<li>Your video should be as close as possible to 360 degrees</li>
<li>The linux command I use to compile this program is: g++ avi-read.cpp -o avi-read -I /usr/include/opencv/ -L /usr/local/lib -lm -lcv -lhighgui -lcvaux</li>
</ol>