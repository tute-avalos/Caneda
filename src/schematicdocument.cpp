/***************************************************************************
 * Copyright (C) 2010 by Gopala Krishna A <krishna.ggk@gmail.com>          *
 *                                                                         *
 * This is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2, or (at your option)     *
 * any later version.                                                      *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this package; see the file COPYING.  If not, write to        *
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,   *
 * Boston, MA 02110-1301, USA.                                             *
 ***************************************************************************/

#include "schematicdocument.h"

#include "cgraphicsscene.h"
#include "schematiccontext.h"
#include "schematicview.h"
#include "settings.h"
#include "statehandler.h"
#include "xmlschematic.h"

#include "dialogs/exportdialog.h"
#include "dialogs/settingsdialog.h"

#include <QFileInfo>
#include <QPainter>
#include <QPrinter>

#include <cmath>

namespace Caneda
{
    SchematicDocument::SchematicDocument()
    {
        m_cGraphicsScene = new CGraphicsScene(this);
        connect(m_cGraphicsScene, SIGNAL(changed()), this,
                SLOT(emitDocumentChanged()));
        connect(m_cGraphicsScene->undoStack(), SIGNAL(canUndoChanged(bool)),
                this, SLOT(emitDocumentChanged()));
        connect(m_cGraphicsScene->undoStack(), SIGNAL(canRedoChanged(bool)),
                this, SLOT(emitDocumentChanged()));
        connect(m_cGraphicsScene, SIGNAL(selectionChanged()), this,
                SLOT(emitDocumentChanged()));
    }

    SchematicDocument::~SchematicDocument()
    {
    }

    // Interface implementation
    IContext* SchematicDocument::context()
    {
        return SchematicContext::instance();
    }

    bool SchematicDocument::isModified() const
    {
        return m_cGraphicsScene->isModified();
    }

    bool SchematicDocument::canUndo() const
    {
        return m_cGraphicsScene->undoStack()->canUndo();
    }

    bool SchematicDocument::canRedo() const
    {
        return m_cGraphicsScene->undoStack()->canRedo();
    }

    void SchematicDocument::undo()
    {
        m_cGraphicsScene->undoStack()->undo();
    }

    void SchematicDocument::redo()
    {
        m_cGraphicsScene->undoStack()->redo();
    }

    QUndoStack* SchematicDocument::undoStack()
    {
        return m_cGraphicsScene->undoStack();
    }

    bool SchematicDocument::canCut() const
    {
        QList<QGraphicsItem*> qItems = m_cGraphicsScene->selectedItems();
        QList<CGraphicsItem*> schItems = filterItems<CGraphicsItem>(qItems);

        return schItems.isEmpty() == false;
    }

    bool SchematicDocument::canCopy() const
    {
        return SchematicDocument::canCut();
    }

    bool SchematicDocument::canPaste() const
    {
        return true;
    }

    void SchematicDocument::cut()
    {
        QList<QGraphicsItem*> qItems = m_cGraphicsScene->selectedItems();
        QList<CGraphicsItem*> schItems = filterItems<CGraphicsItem>(qItems);

        if(!schItems.isEmpty()) {
            m_cGraphicsScene->cutItems(schItems);
        }
    }

    void SchematicDocument::copy()
    {
        QList<QGraphicsItem*> qItems = m_cGraphicsScene->selectedItems();
        QList<CGraphicsItem*> schItems = filterItems<CGraphicsItem>(qItems);

        if(!schItems.isEmpty()) {
            m_cGraphicsScene->copyItems(schItems);
        }
    }

    void SchematicDocument::paste()
    {
        StateHandler::instance()->slotHandlePaste();
    }

    void SchematicDocument::selectAll()
    {
        QPainterPath path;
        path.addRect(m_cGraphicsScene->sceneRect());
        m_cGraphicsScene->setSelectionArea(path);
    }

    bool SchematicDocument::printSupportsFitInPage() const
    {
        return true;
    }

    void SchematicDocument::print(QPrinter *printer, bool fitInView)
    {
        QPainter p(printer);
        p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

        const bool fullPage = printer->fullPage();

        const bool viewGridStatus = Settings::instance()->currentValue("gui/gridVisible").value<bool>();
        Settings::instance()->setCurrentValue("gui/gridVisible", false);

        const QRectF diagramRect = m_cGraphicsScene->imageBoundingRect();
        if(fitInView) {
            m_cGraphicsScene->render(&p,
                    QRectF(), // Dest rect
                    diagramRect, // Src rect
                    Qt::KeepAspectRatio);
        }
        else {
            //Printing on one or more pages
            QRectF printedArea = fullPage ? printer->paperRect() : printer->pageRect();

            const int horizontalPages =
                int(std::ceil(diagramRect.width() / printedArea.width()));
            const int verticalPages =
                int(std::ceil(diagramRect.height() / printedArea.height()));

            QList<QRectF> pagesToPrint;

            //The schematic is printed on a grid of sheets running from top-bottom, left-right.
            qreal yOffset = 0;
            for(int y = 0; y < verticalPages; ++y) {
                //Runs through the sheets of the line
                qreal xOffset = 0;
                for(int x = 0; x < horizontalPages; ++x) {
                    const qreal width = qMin(printedArea.width(), diagramRect.width() - xOffset);
                    const qreal height = qMin(printedArea.height(), diagramRect.height() - yOffset);
                    pagesToPrint << QRectF(xOffset, yOffset, width, height);
                    xOffset += printedArea.width();
                }

                yOffset += printedArea.height();
            }

            for (int i = 0; i < pagesToPrint.size(); ++i) {
                const QRectF rect = pagesToPrint.at(i);
                m_cGraphicsScene->render(&p,
                        rect.translated(-rect.topLeft()), // dest - topleft at (0, 0)
                        rect.translated(diagramRect.topLeft()), // src
                        Qt::KeepAspectRatio);
                if(i != (pagesToPrint.size() - 1)) {
                    printer->newPage();
                }
            }
        }

        Settings::instance()->setCurrentValue("gui/gridVisible", viewGridStatus);
    }

    bool SchematicDocument::load(QString *errorMessage)
    {
        QFileInfo info(fileName());

        if(info.suffix() == "xsch") {
            XmlSchematic *format = new XmlSchematic(this);
            return format->load();
        }

        if (errorMessage) {
            *errorMessage = tr("Unknown file format!");
        }

        return false;
    }

    bool SchematicDocument::save(QString *errorMessage)
    {
        if(fileName().isEmpty()) {
            if (errorMessage) {
                *errorMessage = tr("Empty file name");
            }
            return false;
        }

        QFileInfo info(fileName());

        // Correct the extension.
        if(QString(info.suffix()).isEmpty()) {
            setFileName(fileName() + ".xsch");
            info = QFileInfo(fileName());
        }

        if(info.suffix() == "xsch") {
            XmlSchematic *format = new XmlSchematic(this);
            if(!format->save()) {
                return false;
            }

            m_cGraphicsScene->undoStack()->clear();
            return true;
        }

        if(errorMessage) {
            *errorMessage = tr("Unknown file format!");
        }

        return false;
    }

    void SchematicDocument::exportImage()
    {
        ExportDialog *d = new ExportDialog(this, m_cGraphicsScene);
        d->exec();
    }

    void SchematicDocument::documentSettings()
    {
        QList<SettingsPage *> wantedPages;
        SettingsPage *page = new SchematicDocumentConfigurationPage(cGraphicsScene());
        wantedPages << page;
        page = new SimulationConfigurationPage();
        wantedPages << page;

        SettingsDialog *d = new SettingsDialog(wantedPages, "Configure Document");
        d->exec();
    }

    IView* SchematicDocument::createView()
    {
        return new SchematicView(this);
    }

    void SchematicDocument::updateSettingsChanges()
    {
    }

    // End of Interface implemention.
    CGraphicsScene* SchematicDocument::cGraphicsScene() const
    {
        return m_cGraphicsScene;
    }

} // namespace Caneda
