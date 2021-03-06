//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//  Copyright 2016 National Technology & Engineering Solutions of Sandia, LLC (NTESS).
//  Copyright 2016 UT-Battelle, LLC.
//  Copyright 2016 Los Alamos National Security.
//
//  Under the terms of Contract DE-NA0003525 with NTESS,
//  the U.S. Government retains certain rights in this software.
//
//  Under the terms of Contract DE-AC52-06NA25396 with Los Alamos National
//  Laboratory (LANL), the U.S. Government retains certain rights in
//  this software.
//============================================================================
#ifndef vtk_m_rendering_ColorLegendAnnotation_h
#define vtk_m_rendering_ColorLegendAnnotation_h

#include <vtkm/rendering/vtkm_rendering_export.h>

#include <vtkm/rendering/Camera.h>
#include <vtkm/rendering/Canvas.h>
#include <vtkm/rendering/Color.h>
#include <vtkm/rendering/TextAnnotationScreen.h>
#include <vtkm/rendering/WorldAnnotator.h>

namespace vtkm
{
namespace rendering
{

class VTKM_RENDERING_EXPORT ColorLegendAnnotation
{
private:
  vtkm::Float32 FontScale;
  vtkm::rendering::Color LabelColor;
  std::vector<std::string> Labels;
  std::vector<TextAnnotationScreen*> Annot;
  std::vector<vtkm::rendering::Color> ColorSwatchList;

public:
  ColorLegendAnnotation();
  ~ColorLegendAnnotation();

  void Clear();
  void AddItem(const std::string& label, vtkm::rendering::Color color);

  void SetLabelColor(vtkm::rendering::Color c) { this->LabelColor = c; }

  void SetLabelFontScale(vtkm::Float32 s)
  {
    this->FontScale = s;
    for (unsigned int i = 0; i < this->Annot.size(); i++)
      this->Annot[i]->SetScale(s);
  }

  virtual void Render(const vtkm::rendering::Camera&,
                      const vtkm::rendering::WorldAnnotator& annotator,
                      vtkm::rendering::Canvas& canvas);
};
}
} //namespace vtkm::rendering

#endif // vtk_m_rendering_ColorLegendAnnotation_h
