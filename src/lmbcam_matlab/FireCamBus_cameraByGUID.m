function camPointer = FireCamBus_cameraByGUID( busPointer, guidString)
% documentation...
%

camPointer = lmbcam_matlab( 'FireCamBus_cameraByGUID', busPointer, guidString);
