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

#include "NodeSolution.hh"
#include "Region.hh"
#include "Node.hh"
#include "Vector.hh"
#include "Device.hh"
#include "dsAssert.hh"
#include "GeometryStream.hh"
#include "ModelErrors.hh"


#include <sstream>

template <typename DoubleType>
NodeSolution<DoubleType>::NodeSolution(const std::string &nm, RegionPtr rp)
    :
        NodeModel(nm, rp, NodeModel::SCALAR),
        parentModel()
{
#if 0
    os << "creating NodeSolution " << nm << "\n";
#endif
}

template <typename DoubleType>
NodeSolution<DoubleType>::NodeSolution(const std::string &nm, RegionPtr rp, NodeModelPtr nmp)
    :
        // dirty hack
        NodeModel(nm, rp, NodeModel::SCALAR),
        parentModel(nmp)
{
  parentModelName = parentModel.lock()->GetName();
  RegisterCallback(parentModelName);
#if 0
    os << "creating NodeSolution " << nm << " with parent " << parentModel->GetName() << "\n";
#endif
}

template <typename DoubleType>
NodeModelPtr NodeSolution<DoubleType>::CreateNodeSolution(const std::string &nm, RegionPtr rp)
{
  NodeModel *p = new NodeSolution(nm, rp);
  return p->GetSelfPtr();
}

template <typename DoubleType>
NodeModelPtr NodeSolution<DoubleType>::CreateNodeSolution(const std::string &nm, RegionPtr rp, NodeModelPtr nmp)
{
  NodeModel *p = new NodeSolution(nm, rp, nmp);
  return p->GetSelfPtr();
}

template <typename DoubleType>
void NodeSolution<DoubleType>::calcNodeScalarValues() const
{
    if (!parentModelName.empty())
    {
#if 0
        os << "updating NodeSolution " << GetName() << " from parent " << parentModel->GetName() << "\n";
#endif
        ConstNodeModelPtr nmp = GetRegion().GetNodeModel(parentModelName);
        if (!parentModel.expired())
        {
          parentModel.lock()->template GetScalarValues<DoubleType>();
        }
        else if (nmp)
        {
          parentModel.reset();
          dsErrors::ChangedModelModelDependency(GetRegion(), parentModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::NODE, OutputStream::INFO);
          parentModelName.clear();
        }
        else
        {
          dsErrors::MissingModelModelDependency(GetRegion(), parentModelName, dsErrors::ModelInfo::NODE, GetName(), dsErrors::ModelInfo::NODE, OutputStream::FATAL);
        }
    }
}

template <typename DoubleType>
void NodeSolution<DoubleType>::setInitialValues()
{
    DefaultInitializeValues();
}

template <typename DoubleType>
void NodeSolution<DoubleType>::Serialize(std::ostream &of) const
{
  if (!parentModelName.empty())
  {
    of << "DATAPARENT \"" << parentModelName << "\"";
  }
  else if (this->IsUniform())
  {
    of << "UNIFORM " << GetUniformValue<DoubleType>() << "";
  }
  else
  {
    of << "DATA";
    const NodeScalarList<DoubleType> &vals = this->GetScalarValues<DoubleType>();
    for (size_t i = 0; i < vals.size(); ++i)
    {
      of << "\n" << vals[i];
    }
  }
}

template class NodeSolution<double>;

