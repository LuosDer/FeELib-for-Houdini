
#pragma once

#ifndef __GFE_AttributeInterpRay_h__
#define __GFE_AttributeInterpRay_h__

#include "GFE/GFE_AttributeInterpRay.h"

#include "GFE/GFE_GeoFilter.h"



#include "GA/GA_AttributeRefMap.h"
#include "GA/GA_WeightedSum.h"
#include "GU/GU_RayIntersect.h"



class GFE_AttributeInterpRay : public GFE_AttribFilterWithRef0 {

public:
    using GFE_AttribFilterWithRef0::GFE_AttribFilterWithRef0;


    void
        setComputeParm(
        )
    {
        setHasComputed();
        this->cuspAngleDegrees = cuspAngleDegrees;
    }


private:

    virtual bool
        computeCore() override
    {
        if (groupParser.isEmpty())
            return true;

        GA_AttributeRefMap attribRefmap;
        GA_AttributeFilter filter = GA_AttributeFilter::selectFloatTuple();
        filter = GA_AttributeFilter::selectAnd(filter, GA_AttributeFilter::selectAnd(filter,  ););
        attribRefmap.bind(*geoRef0, *geoRef0);
        // Add all floating point vertex attributes
        attribRefmap.append(filter, GA_ATTRIB_VERTEX);
        // Add all floating point point attributes
        attribRefmap.append(filter, GA_ATTRIB_POINT);
        
        if (1)
        {
            UTparallelFor(groupParser.getPointSplittableRange(), [this, &attribRefmap](const GA_SplittableRange& r)
            {
                GU_RayIntersect rayIntersect(geoRef0->asGU_Detail());
                GU_RayInfo rayInfo(maxDist);
                
                GA_PageHandleT<UT_Vector3T<fpreal64>, fpreal64, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> pos_ph(geo->getP());
                GA_PageHandleT<UT_Vector3T<fpreal64>, fpreal64, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> dir_ph(geo->getP());
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        pos_ph.setPage(start);
                        dir_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            rayIntersect.sendRay(pos_ph.value(elemoff), dir_ph.value(elemoff), rayInfo);
                            const GEO_ConstPrimitiveP& hitPrimT = rayInfo.myPrim();
                            const GEO_Primitive& hitPrim = *hitPrimT;
                            

                            hitPrim.evaluateInteriorPoint(0, attribRefmap, rayInfo.myU, rayInfo.myV);
                            
                            
                            // Get the interpolation vertex offsets and weights of the primitive
                            // at the parametric coordinates (u,v,w).  w is zero for polygon
                            // primitives.
                            UT_Array<GA_Offset> offsetarray;
                            UT_FloatArray weightarray;
                            hitPrim.computeInteriorPointWeights(offsetarray, weightarray, rayInfo.myU, rayInfo.myV);
                            GA_WeightedSum sum;
                            sum.rewind();
                            attribRefmap.startSum(sum, GA_ATTRIB_VERTEX, destvtxoff);
                            for (exint i = 0; i < offsetarray.size(); ++i)
                            {
                                attribRefmap.addSumValue(sum, GA_ATTRIB_VERTEX, destvtxoff, GA_ATTRIB_VERTEX, offsetarray(i), weightarray(i));
                            }
                            attribRefmap.finishSum(sum, GA_ATTRIB_VERTEX, destvtxoff);
                            // Special case for point position on quadrics, volumes, etc.,
                            // by excluding the types we know are fine.
                            GA_PrimitiveTypeId type = srcprim->getTypeId();
                            if (!(  type == GA_PRIMPOLY      || type == GA_PRIMPOLYSOUP  ||
                                    type == GA_PRIMBEZCURVE  || type == GA_PRIMBEZSURF   ||
                                    type == GA_PRIMMESH      || type == GA_PRIMNURBCURVE ||
                                    type == GA_PRIMNURBSURF  || type == GA_PRIMPART      ||
                                    type == GA_PRIMTRISTRIP  || type == GA_PRIMTRIFAN    ||
                                    type == GA_PRIMTRIBEZIER || type == GA_PRIMTETRAHEDRON))
                            {
                                UT_Vector4 ptpos;
                                hitPrim.evaluateInteriorPoint(ptpos, rayInfo.myU, rayInfo.myV);
                                dest.setPos4(destptoff, ptpos);
                            }
                            // Do something with destvtxoff
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        else
        {
            UTparallelFor(groupParser.getPointSplittableRange(), [this, &attribRefmap](const GA_SplittableRange& r)
            {
                GU_RayIntersect rayIntersect(geoRef0->asGU_Detail());
                GU_MinInfo minInfo(maxDist);

                GA_PageHandleT<UT_Vector3T<fpreal64>, fpreal64, true, false, const GA_Attribute, const GA_ATINumeric, const GA_Detail> pos_ph(geo->getP());
                for (GA_PageIterator pit = r.beginPages(); !pit.atEnd(); ++pit)
                {
                    GA_Offset start, end;
                    for (GA_Iterator it(pit.begin()); it.blockAdvance(start, end); )
                    {
                        pos_ph.setPage(start);
                        for (GA_Offset elemoff = start; elemoff < end; ++elemoff)
                        {
                            rayIntersect.minimumPoint(pos_ph.value(elemoff), minInfo);
                            minInfo.u2 
                            minInfo.v2
                            
                        }
                    }
                }
            }, subscribeRatio, minGrainSize);
        }
        return true;
    }



public:
    float maxDist = 1e10;

private:
    exint subscribeRatio = 64;
    exint minGrainSize = 64;

}; // End of class GFE_AttributeInterpRay












#endif





