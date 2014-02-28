function camPointer = FireCamBus_cameraByIndex( busPointer, index)
% documentation...
%

camPointer = lmbcam_matlab( 'FireCamBus_cameraByIndex', busPointer, index);
