/**************************************************************************
**       Title: 
**    $RCSfile: FireCamBusRegistry.hh,v $
**   $Revision: 1.2 $$Name:  $
**       $Date: 2003/12/05 14:14:55 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: FireCamBusRegistry.hh,v $
**  Revision 1.2  2003/12/05 14:14:55  mechnich
**  added missing comments and updated Doxyfile
**
**  Revision 1.1  2003/10/01 13:33:11  pigorsch
**  - initial revision
**
**
**
**************************************************************************/

#ifndef FIRECAMBUSREGISTRY_HH
#define FIRECAMBUSREGISTRY_HH

// stl includes
#include <map>
#include <utility> // std::pair

// firewire includes
#include <libdc1394/dc1394_control.h>

namespace liblmbcam
{

  class FireCamBus;
  
  /*======================================================================*/
  /*!
   *  \class FireCamBusRegistry FireCamBusRegistry.hh
   *  \brief The FireCamBusRegistry class tracks instances of FireCamBus
   #         objects
   *
   *  (description)
   */
  /*======================================================================*/
  class FireCamBusRegistry
  {
  public:
    static FireCamBusRegistry* instance();
    
    void registerBus(raw1394handle_t handle, FireCamBus* bus);
    void deregisterBus(FireCamBus* bus);

    FireCamBus* bus(raw1394handle_t handle);
    
  protected:
    FireCamBusRegistry();
    
  private:
    static FireCamBusRegistry* _instance;
    
    std::vector<std::pair<raw1394handle_t, FireCamBus*> > _busses;
  };

}


#endif // FIRECAMBUSREGISTRY_HH
