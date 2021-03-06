// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "Boxes/imagebox.h"

#include <QMenu>

#include "FileCacheHandlers/imagecachehandler.h"
#include "fileshandler.h"
#include "filesourcescache.h"
#include "GUI/edialogs.h"
#include "typemenu.h"
#include "paintbox.h"

ImageFileHandler* imageFileHandlerGetter(const QString& path) {
    return FilesHandler::sInstance->getFileHandler<ImageFileHandler>(path);
}

ImageBox::ImageBox() : BoundingBox("Image", eBoxType::image),
    mFileHandler(this,
                 [](const QString& path) {
                     return imageFileHandlerGetter(path);
                 },
                 [this](ImageFileHandler* obj) {
                     return fileHandlerAfterAssigned(obj);
                 },
                 [this](ConnContext& conn, ImageFileHandler* obj) {
                     fileHandlerConnector(conn, obj);
                 }) {
}

ImageBox::ImageBox(const QString &filePath) : ImageBox() {
    setFilePath(filePath);
}

void ImageBox::fileHandlerConnector(ConnContext &conn, ImageFileHandler *obj) {
    conn << connect(obj, &ImageFileHandler::pathChanged,
                    this, &ImageBox::prp_afterWholeInfluenceRangeChanged);
    conn << connect(obj, &ImageFileHandler::reloaded,
                    this, &ImageBox::prp_afterWholeInfluenceRangeChanged);
}

void ImageBox::fileHandlerAfterAssigned(ImageFileHandler *obj) {
    Q_UNUSED(obj);
}

void ImageBox::writeBoundingBox(eWriteStream& dst) const {
    BoundingBox::writeBoundingBox(dst);
    dst << mFileHandler.path();
}

void ImageBox::readBoundingBox(eReadStream& src) {
    BoundingBox::readBoundingBox(src);
    QString path; src >> path;
    setFilePath(path);
}

void ImageBox::setFilePath(const QString &path) {
    mFileHandler.assign(path);

    rename(path.split("/").last());
    prp_afterWholeInfluenceRangeChanged();
}

void ImageBox::reload() {
    if(mFileHandler) mFileHandler->reloadAction();
}

void ImageBox::setupCanvasMenu(PropertyMenu * const menu) {
    if(menu->hasActionsForType<ImageBox>()) return;
    menu->addedActionsForType<ImageBox>();

    const PropertyMenu::PlainSelectedOp<ImageBox> reloadOp =
    [](ImageBox * box) {
        box->reload();
    };
    menu->addPlainAction("Reload", reloadOp);

    const PropertyMenu::PlainSelectedOp<ImageBox> setSrcOp =
    [](ImageBox * box) {
        box->changeSourceFile();
    };
    menu->addPlainAction("Set Source File...", setSrcOp);

    menu->addSeparator();

    const PropertyMenu::PlainSelectedOp<ImageBox> createPaintObj =
    [](ImageBox * box) {
        const qptr<ImageBox> boxPtr = box;
        const auto loader = [boxPtr]() {
            if(!boxPtr) return;
            const auto parent = boxPtr->getParentGroup();
            if(!parent) return;
            const auto img = boxPtr->mFileHandler->getImage();
            if(!img) return;
            const auto paintObj = enve::make_shared<PaintBox>();
            paintObj->getSurface()->loadPixmap(img);
            boxPtr->copyBoundingBoxDataTo(paintObj.get());
            parent->addContained(paintObj);
        };
        if(box->mFileHandler->hasImage()) {
            loader();
        } else {
            const auto task = box->mFileHandler->scheduleLoad();
            if(task) task->addDependent({loader, nullptr});
        }
    };
    menu->addPlainAction("Create Paint Object", createPaintObj);

    BoundingBox::setupCanvasMenu(menu);
}

void ImageBox::changeSourceFile() {
    const QString filters = FileExtensions::imageFilters();
    QString importPath = eDialogs::openFile("Change Source", mFileHandler.path(),
                                            "Image Files (" + filters + " *.ora)");
    if(!importPath.isEmpty()) setFilePath(importPath);
}

void ImageBox::setupRenderData(const qreal relFrame,
                               BoxRenderData * const data,
                               Canvas* const scene) {
    BoundingBox::setupRenderData(relFrame, data, scene);
    const auto imgData = static_cast<ImageBoxRenderData*>(data);
    if(mFileHandler->hasImage()) {
        imgData->setContainer(mFileHandler->getImageContainer());
    } else {
        const auto loader = mFileHandler->scheduleLoad();
        if(loader) loader->addDependent(imgData);
    }
}

stdsptr<BoxRenderData> ImageBox::createRenderData() {
    return enve::make_shared<ImageBoxRenderData>(mFileHandler, this);
}

void ImageBoxRenderData::loadImageFromHandler() {
    if(fSrcCacheHandler) {
        setContainer(fSrcCacheHandler->getImageContainer());
    }
}
