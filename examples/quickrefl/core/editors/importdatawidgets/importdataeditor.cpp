// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include "importdataeditor.h"
#include "realdatamodel.h"
#include <QLabel>
#include <QVBoxLayout>

ImportDataEditor::ImportDataEditor(RealDataModel* model, QWidget* parent)
    : QWidget(parent), model(model)
{
    auto layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Hello World!"));
}
