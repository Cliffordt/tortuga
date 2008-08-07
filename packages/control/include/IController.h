/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/include/IController.h
 */

#ifndef RAM_CONTROL_ICONTROLLER_07_03_2007
#define RAM_CONTROL_ICONTROLLER_07_03_2007

// STD Includes
#include <string>

// Library Includes
#include <boost/shared_ptr.hpp>

// Project Includes
#include "core/include/Event.h"
#include "core/include/Subsystem.h"
#include "math/include/Quaternion.h"

// Must Be Included last
#include "control/include/Export.h"

namespace ram {
namespace control {

class IController;
typedef boost::shared_ptr<IController> IControllerPtr;
    
class RAM_EXPORT IController : public core::Subsystem
{
public:
    /**
     * \defgroup Events IController Events
     */
    /* @{ */
    
    /** When the controller desired depth changes
     *
     *  Type is ram::math::NumbericEvent;
     */
    static const core::Event::EventType DESIRED_DEPTH_UPDATE;

    /** When the desired orientation changes (ram::math::OrientationEvent) */
    static const core::Event::EventType DESIRED_ORIENTATION_UPDATE;

    /** When the vehicle reaches the depth set by the controller
     *
     *  Type is ram::math::NumbericEvent;
     */
    static const core::Event::EventType AT_DEPTH;

    /** When the vehicle reaches the orientation set by the controller
     *
     *  Type is ram::math::OrientationEvent
     */
    static const core::Event::EventType AT_ORIENTATION;

    /* @{ */
    
    /** Set the current speed, clamped between -5 and 5 */
    virtual void setSpeed(double speed) = 0;

    /** How fast the vehicle is going side to side (positive = right) */
    virtual void setSidewaysSpeed(double speed) = 0;

    /** Sets the current heading in degrees off north */
//    virtual void setHeading(double degrees) = 0;

    /** Sets the desired depth of the sub in meters */
    virtual void setDepth(double depth) = 0;

    /** Gets the current speed, a value between -5 and 5 */
    virtual double getSpeed() = 0;

    /** Gets the current sideways speed
     *
     *  @return
     *      A value between -5 (left) and 5 (right)
     */
    virtual double getSidewaysSpeed() = 0;
    
    /** The current heading of th*/
//    virtual double getHeading() = 0;

    /** Current desired depth of the sub in meters */
    virtual double getDepth() = 0;
    
    /** Grab current estimated depth*/
    virtual double getEstimatedDepth() = 0;
    
    /** Grab current estimated depth velocity (depthDot)*/
    virtual double getEstimatedDepthDot() = 0;

    /** Yaws the desired vehicle state by the desired number of degrees */
    virtual void yawVehicle(double degrees) = 0;

    /** Pitches the desired vehicle state by the desired number of degrees */
    virtual void pitchVehicle(double degrees) = 0;

    /** Rolls the desired vehicle state by the desired number of degrees */
    virtual void rollVehicle(double degrees) = 0;

    /** Gets the current desired orientation */
    virtual math::Quaternion getDesiredOrientation() = 0;
    
    /** Return to the home orientation */
    virtual inline void setHomeOrientation() { setDesiredOrientation(math::Quaternion::IDENTITY); }
	
    /** Sets the current desired orientation */
    virtual void setDesiredOrientation(math::Quaternion) = 0;
    
    /** Returns true if the vehicle is at the desired depth */
    virtual bool atDepth() = 0;

    /** Returns true if the vehicle is at the desired orientation */
    virtual bool atOrientation() = 0;

    /** Loads current orientation into desired (fixes offset in roll and pitch) */
    virtual void holdCurrentHeading() = 0;
    
    virtual void setBuoyantTorqueCorrection(double x, double y, double z) =0;
    
protected:
    IController(std::string name,
                core::EventHubPtr eventHub = core::EventHubPtr());
};
    
} // namespace control
} // namespace ram

#endif // RAM_CONTROL_ICONTROLLER_07_03_2007
