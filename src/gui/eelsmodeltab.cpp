/**
 * This file is part of eelsmodel.
 *
 * eelsmodel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * eelsmodel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eelsmodel.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jo Verbeeck, Ruben Van Boxem
 * Copyright: 2002-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel - gui/eelsmodeltab.cpp
 **/

#include "src/gui/eelsmodeltab.h"

#include <QActionGroup>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QSplitter>
#include <QStatusBar>
#include <QToolBar>

#include "src/core/image.h"
#include "src/core/model.h"

#include "src/gui/componentmaintenance.h"
#include "src/gui/fitter_dialog.h"
#include "src/gui/imagedisplay.h"

//TODO consolidate these two once Spectrum is properly implemented
EELSModelTab::EELSModelTab(Spectrum* spectrum, QWidget* parent)
: QMainWindow(parent),
  multi(false),
  spectrum(spectrum),
  model(spectrum)
{
  setAttribute(Qt::WA_DeleteOnClose); // delete this widget when tab is closed

  createMenuBar();
  createLayout();
  createToolBar(); // needs things created in createLayout
}

EELSModelTab::EELSModelTab(Image* image, QWidget* parent)
: QMainWindow(parent),
  multi(true),
  spectrum(image->getmspec()),
  model(image->getmspec())
{
  setAttribute(Qt::WA_DeleteOnClose); // delete this widget when tab is closed

  createToolBar();
  createLayout();
}

void EELSModelTab::createMenuBar()
{
  // Edit actions
  QAction* undoSelection = new QAction(tr("&Undo selection"), this);
  undoSelection->setStatusTip(tr("Undo selection"));
  undoSelection->setWhatsThis(tr("Undo selection"));
  connect(undoSelection, SIGNAL(activated()), this, SLOT(slotEditUndoSelection()));

  QAction* exclude = new QAction(tr("Exclude points"), this);
  exclude->setStatusTip(tr("Exlude selected points from fitting"));
  exclude->setWhatsThis(tr("Exlude selected points from fitting"));
  connect(exclude, SIGNAL(activated()), this, SLOT(slotEditExclude()));

  QAction* resetExclude = new QAction(tr("Reset Exclude points"), this);
  resetExclude->setStatusTip(tr("Reset exclude region, \n either on a selection or on the\n whole spectrum"));
  resetExclude->setWhatsThis(tr("Reset exclude region, \n either on a selection or on the\n whole spectrum"));
  connect(resetExclude, SIGNAL(activated()), this, SLOT(slotEditResetExclude()));

  //view actions
/* there is no other way to perform the toolbar actions
  viewToggleToolBar = new QAction(tr("Toggle Toolbar"), this);
  viewToggleToolBar->setStatusTip(tr("Enables/disables the toolbar"));
  viewToggleToolBar->setWhatsThis(tr("Toggle Toolbar\n\nEnables/disables the toolbar"));
  viewToggleToolBar->setCheckable(true);
  connect(viewToggleToolBar, SIGNAL(toggled(bool)), this, SLOT(slotViewToolBar(bool)));
*/

  //model actions
  QAction* modelNew = new QAction(tr("New Model"), this);
  modelNew->setStatusTip(tr("New Model"));
  modelNew->setWhatsThis(tr("New Model\n\nCreate a new model"));
  connect(modelNew, SIGNAL(activated()), this, SLOT(newModel()));

  QAction* modelDETECTOR = new QAction(tr("choose detector"), this);
  modelDETECTOR->setStatusTip(tr("set the detector properties"));
  modelDETECTOR->setWhatsThis(tr("set the detector properties"));
  connect(modelDETECTOR, SIGNAL(activated()), this, SLOT(slotModelDETECTOR()));
  modelDETECTOR->setEnabled(false);

  // The menu bar
  QMenu* editMenu = menuBar()->addMenu("Edit");
  editMenu->addAction(undoSelection);
  editMenu->addAction(exclude);
  editMenu->addAction(resetExclude);

  QMenu* modelMenu = menuBar()->addMenu("Model");
  modelMenu->addAction(modelNew);
  modelMenu->addAction(modelDETECTOR);
}

void EELSModelTab::createLayout()
{
  QSplitter* hsplitter = new QSplitter(Qt::Horizontal);
  QSplitter* leftsplitter = new QSplitter(Qt::Vertical, hsplitter);
  QSplitter* rightsplitter = new QSplitter(Qt::Vertical, hsplitter);
  hsplitter->addWidget(leftsplitter);
  hsplitter->addWidget(rightsplitter);

  hsplitter->setChildrenCollapsible(false);
  leftsplitter->setChildrenCollapsible(false);
  rightsplitter->setChildrenCollapsible(false);

  setCentralWidget(hsplitter);

  rightsplitter->addWidget(new ComponentEditor(model));
  //TODO evil hack needs to be solved at the Spectrum level
  if(multi)
  {
    QLabel* image = new QLabel();
    image->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    image->setPixmap(QPixmap::fromImage(convertmspectoimage(*dynamic_cast<Multispectrum*>(spectrum.get()))));

    leftsplitter->addWidget(image);

    rightsplitter->addWidget(new Graph(dynamic_cast<Multispectrum*>(spectrum.get())));
    rightsplitter->addWidget(new Graph(model.getmultispectrumptr()));
  }
  else
  {
    rightsplitter->addWidget(new Graph(spectrum.get()));
    rightsplitter->addWidget(new Graph(model.getspectrumptr()));
  }

  leftsplitter->addWidget(new fitterWidget());
}

//TODO
void EELSModelTab::createToolBar()
{
  // Exclusively grouped mouse cursor actions
  QActionGroup *group = new QActionGroup(this);
  group->setExclusive(true);

  //the normal mouse button
  QAction* normal = new QAction(QIcon(":/icons/previous.png"), tr("Normal"), group);
  normal->setStatusTip(tr("Normal mode"));
  normal->setWhatsThis(tr("Normal\n\nSets the cursor in the normal mode"));
  normal->setCheckable(true);
  normal->setChecked(true); // this should be the default option
  connect(normal, SIGNAL(triggered()), this, SIGNAL(setNormalMode()));
  //the selection button
  QAction* selection = new QAction(QIcon(":/icons/rectangle.png"), tr("Region Select"), group);
  selection->setStatusTip(tr("Select a region in a graph"));
  selection->setWhatsThis(tr("Region Select\n\nSelect a region in a graph"));
  selection->setCheckable(true);
  connect(selection, SIGNAL(triggered()), this, SIGNAL(setSelectMode()));

  //the zoom button
  QAction* zoom = new QAction(QIcon(":/icons/zoom.png"), tr("&Zoom"), group);
  zoom->setStatusTip(tr("Zoom in on a graph"));
  zoom->setWhatsThis(tr("Zoom\n\nZooms in on a graph"));
  zoom->setCheckable(true);
  connect(zoom, SIGNAL(triggered()), this, SIGNAL(setZoomMode()));

  // ungrouped buttons

  //the display home button
  QAction* home = new QAction(QIcon(":/icons/gohome.png"), tr("Home display"), this);
  home->setStatusTip(tr("Home display"));
  home->setWhatsThis(tr("Home display\n\nResets the graph arrea to show/n the original size"));
  home->setCheckable(false);
  connect(home, SIGNAL(triggered()), this, SIGNAL(toolbar_home()));

  //the link button
  QAction* link = new QAction(QIcon(":/icons/connect_established.png"), tr("Link"), this);
  link->setStatusTip(tr("Link parameters"));
  link->setWhatsThis(tr("Link\n\nLinks parameters together"));
  link->setCheckable(false);
  connect(link, SIGNAL(triggered()), this, SIGNAL(toolbar_link()));

  // The toolbar itself
  QToolBar* basicToolbar = addToolBar("basic tools");
  basicToolbar->addAction(normal);
  basicToolbar->addAction(selection);
  basicToolbar->addAction(zoom);
  basicToolbar->addSeparator(); // seperator between grouped and ungrouped buttons
  basicToolbar->addAction(home);
  basicToolbar->addAction(link);
}

void EELSModelTab::cut() {}
void EELSModelTab::copy() {}
void EELSModelTab::paste() {}
void EELSModelTab::undoSelection() {}
void EELSModelTab::exclude() {}
void EELSModelTab::resetExclude() {}

void EELSModelTab::newModel()
{
  switch(QMessageBox::warning(this, "EELSModel",
                              tr("Creating a new model will replace the current one.\n"
                                 "Save the current model?"),
                              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel))
  {
    case QMessageBox::Save:
      QMessageBox::warning(this, "Unimplemented", "Saving a model is currently unimplemented.");
    case QMessageBox::Discard:
      break; // continue below
    default:
      return; // the user cancelled
  }

  //TODO evil hacks need to be solved at the Spectrum level
  if(multi)
  {
    model = Model(dynamic_cast<Multispectrum*>(spectrum.get()));
    //TODO: is this even possible?
    //update model wheneverspectrum changes
    connect(imagedisplay, SIGNAL(curr_spec_update()), model_graph, SLOT(slot_model_update()));
  }
  else
    model = Model(spectrum.get());

  model.calculate();
  //model.display(getworkspaceptr());
  //model->printcomponents();
  //emit enablemodel(true);
}

void EELSModelTab::setDetector() {}

void EELSModelTab::setNormalMode()
{
  spectrum_graph->mouseMode = Graph::normal;
  model_graph->mouseMode = Graph::normal;
}
void EELSModelTab::setZoomMode()
{
  spectrum_graph->mouseMode = Graph::zoom;
  model_graph->mouseMode = Graph::zoom;
}
void EELSModelTab::setSelectMode()
{
  spectrum_graph->mouseMode = Graph::select;
  model_graph->mouseMode = Graph::select;
}

void EELSModelTab::closeEvent(QCloseEvent* event)
{
  switch(QMessageBox::warning(this, tr("EELSModel"),
                              tr("This will discard the current model.\n"
                                 "Save the current model?"),
                              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel))
  {
    case QMessageBox::Save:
      QMessageBox::warning(this, "Unimplemented", "Saving a model is currently unimplemented.");
    case QMessageBox::Discard:
      break;
    default: // also cancel
      return;
  }
}
