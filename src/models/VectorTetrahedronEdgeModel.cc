/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#include "VectorTetrahedronEdgeModel.hh"
#include "TetrahedronEdgeSubModel.hh"
#include "Region.hh"
#include "TetrahedronElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"

template <typename DoubleType>
VectorTetrahedronEdgeModel<DoubleType>::VectorTetrahedronEdgeModel(const std::string &edgemodel, RegionPtr rp)
    : TetrahedronEdgeModel(edgemodel + "_x", rp, TetrahedronEdgeModel::SCALAR), elementEdgeModelName(edgemodel), y_ModelName(elementEdgeModelName+ "_y"), z_ModelName(elementEdgeModelName + "_z")
{
  RegisterCallback(edgemodel);
  new TetrahedronEdgeSubModel<DoubleType>(y_ModelName, rp, this->GetSelfPtr(), TetrahedronEdgeModel::SCALAR);
  new TetrahedronEdgeSubModel<DoubleType>(z_ModelName, rp, this->GetSelfPtr(), TetrahedronEdgeModel::SCALAR);
}

template <typename DoubleType>
void VectorTetrahedronEdgeModel<DoubleType>::calcTetrahedronEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  const ConstTetrahedronEdgeModelPtr emp = reg.GetTetrahedronEdgeModel(elementEdgeModelName);
  dsAssert(emp.get(), "UNEXPECTED");

  const ConstTetrahedronEdgeModelPtr tempy = reg.GetTetrahedronEdgeModel(y_ModelName);
  dsAssert(tempy.get(), "UNEXPECTED");

  const ConstTetrahedronEdgeModelPtr tempz = reg.GetTetrahedronEdgeModel(z_ModelName);
  dsAssert(tempz.get(), "UNEXPECTED");

  const ConstTetrahedronList &tl = GetRegion().GetTetrahedronList();

  std::vector<DoubleType> evx(6*tl.size());
  std::vector<DoubleType> evy(6*tl.size());
  std::vector<DoubleType> evz(6*tl.size());

  const TetrahedronElementField &efield = reg.GetTetrahedronElementField();
  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Tetrahedron &tetrahedron = *tl[i];
    const std::vector<Vector<DoubleType> > &v = efield.GetTetrahedronElementField(tetrahedron, *emp);
    for (size_t j = 0; j < 6; ++j)
    {
      const Vector<DoubleType> &vec = v[j];
      evx[6*i + j] = vec.Getx();
      evy[6*i + j] = vec.Gety();
      evz[6*i + j] = vec.Getz();
    }
  }

  SetValues(evx);
  std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempy)->SetValues(evy);
  std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(tempz)->SetValues(evz);
}

template <typename DoubleType>
void VectorTetrahedronEdgeModel<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND vector_element_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -element_model \"" << elementEdgeModelName << "\"";
}

template class VectorTetrahedronEdgeModel<double>;

