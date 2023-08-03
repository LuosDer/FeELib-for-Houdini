
#ifndef __SOP_FeE_VolumeProject_1_0_h__
#define __SOP_FeE_VolumeProject_1_0_h__

#include <SOP/SOP_Node.h>
#include <UT/UT_StringHolder.h>

namespace SOP_FeE_VolumeProject_1_0_Namespace {

	class SOP_FeE_VolumeProject_1_0 : public SOP_Node
	{
    
	public:
		static PRM_Template *buildTemplates();
		static OP_Node *myConstructor(OP_Network *net, const char *name, OP_Operator *op)
		{
			OP_Node* newOp = new SOP_FeE_VolumeProject_1_0(net, name, op);
			//newOp->setColor(UT_Color(UT_ColorType::UT_RGB, 0.8, 0.5, 0.5));
			newOp->setNodeShape("squared");
			//newOp->setUserData("nodeshape", "tilted", false);
			return newOp;
		}

		static const UT_StringHolder theSOPTypeName;
    
		const SOP_NodeVerb *cookVerb() const override;

	protected:
		SOP_FeE_VolumeProject_1_0(OP_Network *net, const char *name, OP_Operator *op)
			: SOP_Node(net, name, op)
		{
			//mySopFlags.setManagesDataIDs(true);
		}
    
		~SOP_FeE_VolumeProject_1_0() override {}

		OP_ERROR cookMySop(OP_Context &context) override
		{
			return cookMyselfAsVerb(context);
		}

		const char* inputLabel(unsigned idx) const override
		{
			switch (idx)
			{
			case 0:     return "Volume";
			case 1:     return "Volume";
			case 2:     return "Volume";
			case 3:     return "Volume";
			default:    return "Invalid Source";
			}
		}

		const char* outputLabel(unsigned idx) const override
		{
			switch (idx)
			{
			case 0:     return "Volume";
			default:    return "Invalid Output";
			}
		}

		int isRefInput(unsigned i) const override
		{
			return (i == 3);
		}


	};
} // End SOP_FeE_VolumeProject_1_0_Namespace namespace

#endif
