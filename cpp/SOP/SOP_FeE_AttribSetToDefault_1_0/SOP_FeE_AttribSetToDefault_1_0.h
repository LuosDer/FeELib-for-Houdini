
#ifndef __SOP_FeE_AttribSetToDefault_1_0_h__
#define __SOP_FeE_AttribSetToDefault_1_0_h__

#include <SOP/SOP_Node.h>
#include <UT/UT_StringHolder.h>

namespace SOP_FeE_AttribSetToDefault_1_0_Namespace {
/// This is the SOP class definition.  It doesn't need to be in a separate
/// file like this.  This is just an example of a header file, in case
/// another file needs to reference something in here.
/// You shouldn't have to change anything in here except the name of the class.
class SOP_FeE_AttribSetToDefault_1_0 : public SOP_Node
{
public:
    static PRM_Template *buildTemplates();
    static OP_Node *myConstructor(OP_Network *net, const char *name, OP_Operator *op)
    {
        return new SOP_FeE_AttribSetToDefault_1_0(net, name, op);
    }

    static const UT_StringHolder theSOPTypeName;
    
    const SOP_NodeVerb *cookVerb() const override;

protected:
    SOP_FeE_AttribSetToDefault_1_0(OP_Network *net, const char *name, OP_Operator *op)
        : SOP_Node(net, name, op)
    {
        // All verb SOPs must manage data IDs, to track what's changed
        // from cook to cook.
        mySopFlags.setManagesDataIDs(true);
    }
    
    ~SOP_FeE_AttribSetToDefault_1_0() override {}

    /// Since this SOP implements a verb, cookMySop just delegates to the verb.
    OP_ERROR cookMySop(OP_Context &context) override
    {
        return cookMyselfAsVerb(context);
    }

    const char* inputLabel(unsigned idx) const override
    {
        switch (idx)
        {
        case 0:     return "Mesh with UV";
        default:    return "Invalid Source";
        }
    }

    int isRefInput(unsigned i) const override
    {
        // First or second input both use dotted lines
        return (i != 0);
    }


};
} // End SOP_FeE_AttribSetToDefault_1_0_Namespace namespace

#endif
