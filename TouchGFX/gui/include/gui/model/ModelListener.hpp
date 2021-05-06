#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>
#include "main.h"
#include "types.h"

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }
    virtual void SetText(nav_t GPS_data, nav_t XBEE_data) {}
    //virtual void SetText(uint32_t Lat, uint32_t Long, uint32_t Alt, uint32_t Tim) {}
protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
