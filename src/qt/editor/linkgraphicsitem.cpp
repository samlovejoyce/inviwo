/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2012-2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Main file authors: Peter Steneteg, Sathish Kottravel
 *
 *********************************************************************************/

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>
#include <QVector2D>
#include <inviwo/qt/editor/linkgraphicsitem.h>

namespace inviwo {

LinkGraphicsItem::LinkGraphicsItem(QPointF startPoint, QPointF endPoint, ivec3 color)
    : startPoint_(startPoint), endPoint_(endPoint), color_(color.r, color.g, color.b) {

    setZValue(LINKGRAPHICSITEM_DEPTH);
    QGraphicsDropShadowEffect* processorShadowEffect = new QGraphicsDropShadowEffect();
    processorShadowEffect->setOffset(3.0);
    processorShadowEffect->setBlurRadius(3.0);
    setGraphicsEffect(processorShadowEffect);
}

LinkGraphicsItem::~LinkGraphicsItem() {}

QPainterPath LinkGraphicsItem::obtainCurvePath() const {
    float delta = endPoint_.y() - startPoint_.y();
    QPointF ctrlPoint1 = QPointF((startPoint_.x() + endPoint_.x()) / 2.0, startPoint_.y());
    QPointF ctrlPoint2 = QPointF(endPoint_.x(), startPoint_.y() + delta / 2.0);
    QPainterPath bezierCurve;
    bezierCurve.moveTo(startPoint_);
    bezierCurve.cubicTo(ctrlPoint1, ctrlPoint2, endPoint_);
    return bezierCurve;
}

void LinkGraphicsItem::paint(QPainter* p, const QStyleOptionGraphicsItem* options,
                             QWidget* widget) {
    IVW_UNUSED_PARAM(options);
    IVW_UNUSED_PARAM(widget);

    if (isSelected())
        p->setPen(QPen(Qt::darkRed, 2.0, Qt::DotLine, Qt::RoundCap));
    else
        p->setPen(QPen(color_, 2.0, Qt::DotLine, Qt::RoundCap));

    p->drawPath(obtainCurvePath());
}

QPainterPath LinkGraphicsItem::shape() const {
    QPainterPathStroker pathStrocker;
    pathStrocker.setWidth(10.0);
    return pathStrocker.createStroke(obtainCurvePath());
}

QRectF LinkGraphicsItem::boundingRect() const {
    QPointF topLeft =
        QPointF(std::min(startPoint_.x(), endPoint_.x()), std::min(startPoint_.y(), endPoint_.y()));
    return QRectF(topLeft.x() - 30.0, topLeft.y() - 10.0,
                  abs(startPoint_.x() - endPoint_.x()) + 60.0,
                  abs(startPoint_.y() - endPoint_.y()) + 20.0);
}

LinkConnectionGraphicsItem::LinkConnectionGraphicsItem(ProcessorGraphicsItem* outProcessor,
                                                       ProcessorGraphicsItem* inProcessor)
    : LinkGraphicsItem(outProcessor->getShortestBoundaryPointTo(inProcessor),
                       inProcessor->getShortestBoundaryPointTo(outProcessor))
    , outProcessor_(outProcessor)
    , inProcessor_(inProcessor) {
    setFlags(ItemIsSelectable | ItemIsFocusable);
}

LinkConnectionGraphicsItem::~LinkConnectionGraphicsItem() {}

QRectF LinkConnectionGraphicsItem::boundingRect() const {
    QPointF outc = outProcessor_->pos();
    QPointF inc = inProcessor_->pos();
    QPointF outRight =
        outc +
        mapToParent((outProcessor_->rect().bottomRight() + outProcessor_->rect().topRight()) / 2);
    QPointF outLeft =
        outc +
        mapToParent((outProcessor_->rect().bottomLeft() + outProcessor_->rect().topLeft()) / 2);
    QPointF inRight =
        inc +
        mapToParent((inProcessor_->rect().bottomRight() + inProcessor_->rect().topRight()) / 2);
    QPointF inLeft =
        inc + mapToParent((inProcessor_->rect().bottomLeft() + inProcessor_->rect().topLeft()) / 2);
    QPointF start;
    QPointF stop;

    if (outRight.x() < inLeft.x()) {
        start = outRight;
        stop = inLeft;
    } else if (outLeft.x() > inRight.x()) {
        start = outLeft;
        stop = inRight;
    } else {
        start = outRight;
        stop = inRight;
    }

    QPointF topLeft = QPointF(std::min(start.x(), stop.x()), std::min(start.y(), stop.y()));
    QPointF bottomRight = QPointF(std::max(start.x(), stop.x()), std::max(start.y(), stop.y()));
    return QRectF(topLeft.x() - 30, topLeft.y() - 10, bottomRight.x() - topLeft.x() + 70,
                  bottomRight.y() - topLeft.y() + 20);
}

QPainterPath LinkConnectionGraphicsItem::obtainCurvePath() const {
    QPointF outc = outProcessor_->pos();
    QPointF inc = inProcessor_->pos();
    QPointF outRight =
        outc +
        mapToParent((outProcessor_->rect().bottomRight() + outProcessor_->rect().topRight()) / 2);
    QPointF outLeft =
        outc +
        mapToParent((outProcessor_->rect().bottomLeft() + outProcessor_->rect().topLeft()) / 2);
    QPointF inRight =
        inc +
        mapToParent((inProcessor_->rect().bottomRight() + inProcessor_->rect().topRight()) / 2);
    QPointF inLeft =
        inc + mapToParent((inProcessor_->rect().bottomLeft() + inProcessor_->rect().topLeft()) / 2);
    QPointF start;
    QPointF stop;
    QPointF ctrlPointStart;
    QPointF ctrlPointStop;
    QPointF qp = QPointF(1, 0);
    QPainterPath bezierCurve;

    if (outRight.x() <= inLeft.x()) {
        start = outRight;
        stop = inLeft;
        ctrlPointStart = qp;
        ctrlPointStop = -qp;
    } else if (outLeft.x() >= inRight.x()) {
        start = outLeft;
        stop = inRight;
        ctrlPointStart = -qp;
        ctrlPointStop = qp;
    } else {
        start = outRight;
        stop = inRight;
        ctrlPointStart = qp;
        ctrlPointStop = qp;
    }

    float dist =
        1.0f + std::min(50.0f, 2.0f * static_cast<float>(QVector2D(start - stop).length()));
    bezierCurve.moveTo(start);
    bezierCurve.cubicTo(start + dist * ctrlPointStart, stop + dist * ctrlPointStop, stop);
    return bezierCurve;
}

}  // namespace