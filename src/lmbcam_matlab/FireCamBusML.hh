/**************************************************************************
**       Title: 
**    $RCSfile: FireCamBusML.hh,v $
**   $Revision: 1.2 $$Name:  $
**       $Date: 2004/03/22 15:45:16 $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: FireCamBusML.hh,v $
**  Revision 1.2  2004/03/22 15:45:16  ronneber
**  - ugly work-around, because dynamic_cast does not work correctly with
**    dynamic libraries --> replaced with reinterpret_cast
**
**  Revision 1.1  2003/05/13 09:33:56  ronneber
**  initial revision
**
**
**
**************************************************************************/


#include <FireCamBus.hh>

/*-------------------------------------------------------------------------
 *  provide FireCamBus methods with matlab-style parameters
 *-------------------------------------------------------------------------*/
class FireCamBusML : public liblmbcam::FireCamBus
{	
public:
  FireCamBusML( int busIndex)
          : FireCamBus( busIndex)
        {}
  

  void callMethodByName( std::string name,
                         const InputArgs& argsIn, OutputArgs& argsOut)
        {
          if(      name == "FireCamBus_rescan")        
              rescanML(argsIn, argsOut);

          else if( name == "FireCamBus_nCameras")      
              nCamerasML( argsIn, argsOut);

          else if( name == "FireCamBus_cameraByIndex") 
              cameraByIndexML( argsIn, argsOut);

          else if( name == "FireCamBus_cameraByGUID")  
              cameraByGUIDML( argsIn, argsOut);

          else
          {
            char errmsg[1024];
            sprintf( errmsg, "No method '%s' in FireCamBus class!",
                     name.c_str());
            mexErrMsgTxt( errmsg);
          }
        }
  
          
  void rescanML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 0);
          argsOut.assertNArgs( 0);
          rescan();
        }
  
  void nCamerasML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 0);
          argsOut.assertNArgs( 1);
          argsOut.setValueDouble( 0, nCameras());
        }
  
  void cameraByIndexML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 1);
          argsOut.assertNArgs( 1);
          unsigned int index = (unsigned int)argsIn.asDouble( 2);
          argsOut.setValueUint( 0, reinterpret_cast<unsigned int>(
                                    cameraByIndex( index)) );
        }
  
  void cameraByGUIDML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 1);
          argsOut.assertNArgs( 1);
          std::string guid = argsIn.asString( 2);
          argsOut.setValueUint( 0, reinterpret_cast<unsigned int>(
                                    cameraByGUID( guid)) );
        }
  
  liblmbcam::FireCam* findCameraByPointer( unsigned int pointerAsInt)
        {
//          mexPrintf("search for camera with pointer %d\n", pointerAsInt);
          
          for( int i = 0; i < nCameras(); ++i)
          {
//            mexPrintf("camera %d has pointer %d\n", i, (unsigned int)cameraByIndex(i));
            if( (unsigned int)cameraByIndex(i) == pointerAsInt)
            {
              // Arrgh: this should have been a dynamic_cast, but mex
              // seems to crash with this. So use this ugly workaround...
              return reinterpret_cast<liblmbcam::FireCam*>(cameraByIndex(i));
            }
          }
          return NULL;
        }
  

};
