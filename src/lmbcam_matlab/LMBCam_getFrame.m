function image = LMBCam_getFrame( camPointer, index)
% documentation...
%
if nargin==1
  index = lmbcam_matlab( 'LMBCam_mostRecentFrameIndex', camPointer);
  if( index == -1)
    pause(1)
    index = lmbcam_matlab( 'LMBCam_mostRecentFrameIndex', camPointer);
  end
  if( index == -1)
    error( 'hurz:harz',['got no frame from camera within 1 second.\n',...
	    'Maybe you forget to start it with\n' ...
	    'LMBCam_showLiveWindow(cam) or LMBCam_startCamera...'])
  end
end
image = lmbcam_matlab( 'LMBCam_getFrame', camPointer, index);


