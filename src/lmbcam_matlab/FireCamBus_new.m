function busPointer = FireCamBus_new( busIndex)
% documentation...
%

busPointer = lmbcam_matlab( 'FireCamBus_new', 0, busIndex);
