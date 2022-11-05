
#ifndef __SOP_FeEAttribScale_h__
#define __SOP_FeEAttribScale_h__

#include <SOP/SOP_Node.h>
#include <UT/UT_StringHolder.h>

namespace SOP_FeEAttribScale_Namespace {
/// This is the SOP class definition.  It doesn't need to be in a separate
/// file like this.  This is just an example of a header file, in case
/// another file needs to reference something in here.
/// You shouldn't have to change anything in here except the name of the class.
class SOP_FeEAttribScale : public SOP_Node
{
public:
    static PRM_Template *buildTemplates();
    static OP_Node *myConstructor(OP_Network *net, const char *name, OP_Operator *op)
    {
        return new SOP_FeEAttribScale(net, name, op);
    }

    static const UT_StringHolder theSOPTypeName;
    
    const SOP_NodeVerb *cookVerb() const override;

protected:
    SOP_FeEAttribScale(OP_Network *net, const char *name, OP_Operator *op)
        : SOP_Node(net, name, op)
    {
        // All verb SOPs must manage data IDs, to track what's changed
        // from cook to cook.
        mySopFlags.setManagesDataIDs(true);
    }
    
    ~SOP_FeEAttribScale() override {}

    /// Since this SOP implements a verb, cookMySop just delegates to the verb.
    OP_ERROR cookMySop(OP_Context &context) override
    {
        return cookMyselfAsVerb(context);
    }
};
} // End SOP_FeEAttribScale_Namespace namespace

#endif
