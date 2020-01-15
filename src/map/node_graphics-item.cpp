/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "include/map/node-graphics-item.h"
#include "include/map/graph.h"
#include <QtWidgets>
#include <iostream>

std::shared_ptr<MapData> NodeGraphicsItem::_mapdata = nullptr;
NodeGraphicsItem::SelectNodeState NodeGraphicsItem::_sel_node_state = NodeGraphicsItem::SelectNodeState::SEL_START;
NodeId NodeGraphicsItem::_start = 0;
NodeId NodeGraphicsItem::_end = 0;
Graph::Way NodeGraphicsItem::_way = Graph::Way();


/**
 * @brief NodeGraphicsItem::NodeGraphicsItem
 * @param division_id :
 * @param mapdata : 整个地图类
 */
NodeGraphicsItem::NodeGraphicsItem(int division_id, std::shared_ptr<MapData> &mapdata)
    : _division_id(division_id)
{
    if (_mapdata == nullptr) {
        // initializing static members
        _mapdata = mapdata;
        _sel_node_state = SelectNodeState::SEL_START;
    }

    this->setFlags(ItemIsSelectable);
    this->setAcceptHoverEvents(false);

    this->_division_box = _mapdata->getDivisionBox(this->_division_id);
    this->_bounding_box = QRectF(
        this->_division_box.x_min, this->_division_box.y_min,
        this->_division_box.x_max - this->_division_box.x_min,
        this->_division_box.y_max - this->_division_box.y_min);
}

QRectF NodeGraphicsItem::boundingRect() const
{
    return this->_bounding_box;
}

QPainterPath NodeGraphicsItem::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

/**
 * @brief NodeGraphicsItem::getLevelOfDetailsBound
 * @param lod
 * @return
 * 说明：根据边界获得缩放的level
 */
double NodeGraphicsItem::getLevelOfDetailsBound(NodeGraphicsItem::LevelOfDetails lod)
{
    switch (lod) {
    case LevelOfDetails::VERY_FAR_AWAY:  //超远视角
        return 0.02;
    case LevelOfDetails::FAR_AWAY:      //较远的视角
        return 0.05;
    case LevelOfDetails::NORMAL:        //正常视角
        return 0.2;
    case LevelOfDetails::CLOSE:         //较劲视角
        return 0.5;
    case LevelOfDetails::VERY_CLOSE:    //超近视角
        return 1.0;
    default:
        throw std::invalid_argument("invalid LevelOfDetails bound enum");
    }
}


/**
 * @brief NodeGraphicsItem::getPenWidthHighlighted
 * @param lod
 * @return
 * 说明：根据缩放的level获得pen的高亮程度
 */
int NodeGraphicsItem::getPenWidthHighlighted(qreal lod)
{
    if (lod < getLevelOfDetailsBound(LevelOfDetails::VERY_FAR_AWAY))
        return 100;
    else if (lod < getLevelOfDetailsBound(LevelOfDetails::FAR_AWAY))
        return 50;
    else if (lod < getLevelOfDetailsBound(LevelOfDetails::NORMAL))
        return 30;
    else if (lod < getLevelOfDetailsBound(LevelOfDetails::VERY_CLOSE))
        return 20;
    else
        return 10;
}

/**
 * @brief NodeGraphicsItem::getPenWidth
 * @param lod
 * @return
 * 说明：获得线的宽度
 */
int NodeGraphicsItem::getPenWidth(qreal lod)
{
    if (lod < getLevelOfDetailsBound(LevelOfDetails::VERY_FAR_AWAY))
        return 30;
    else if (lod < getLevelOfDetailsBound(LevelOfDetails::FAR_AWAY))
        return 15;
    else if (lod < getLevelOfDetailsBound(LevelOfDetails::NORMAL))
        return 8;
    else if (lod < getLevelOfDetailsBound(LevelOfDetails::VERY_CLOSE))
        return 4;
    else
        return 2;
}

/**
 * @brief NodeGraphicsItem::drawNode
 * @param node
 * @param painter
 * @param lod
 */
void NodeGraphicsItem::drawNode(const Node &node, QPainter &painter, qreal lod)
{
    Q_UNUSED(lod);

    bool useThickPen = false;
    QPointF pf(node.point.x, node.point.y);
    if ((_sel_node_state == SelectNodeState::SEL_END && node.id == _start) ||
        (_sel_node_state == SelectNodeState::WAY_DRAWN && (node.id == _start || node.id == _end))) {
        useThickPen = true;
    }

    if (!useThickPen) {
        int width = getPenWidth(lod);
        painter.drawPoint(pf);
        painter.drawEllipse(pf, width, width);
    } else {
        int width = getPenWidthHighlighted(lod);
        QPen oldPen = painter.pen();
        QPen thickPen = oldPen;
        thickPen.setWidth(width);
        thickPen.setColor(Qt::blue);
        painter.setPen(thickPen);
        painter.drawEllipse(pf, width, width);
        painter.setPen(oldPen);
    }
}

/**
 * @brief NodeGraphicsItem::drawWay
 * @param painter
 * @param lod
 * @说明：画两个node之间的线
 */
void NodeGraphicsItem::drawWay(QPainter &painter, qreal lod)
{
    if (_way.empty())
        return;

    QPen oldPen = painter.pen();
    QPen highlight = oldPen;
    highlight.setColor(Qt::blue);
    highlight.setWidth(getPenWidthHighlighted(lod)); //根据线条的级别

    painter.setPen(highlight);
    //画这条道路
    for (auto &w : _way) {
        drawLink(*w, painter, lod);
    }
    //画完之后，再将画笔置为原来的样子
    painter.setPen(oldPen);
}

/**
 * @brief NodeGraphicsItem::drawLink
 * @param link
 * @param painter
 * @param lod
 * 说明：画两个节点之间的这条线
 */
void NodeGraphicsItem::drawLink(const Link &link, QPainter &painter, qreal lod)
{
    Q_UNUSED(lod);

    if (link.polyline.parts.empty())
        throw std::invalid_argument("link.polyline.parts is empty!");

    for (auto &part : link.polyline.parts) {
        std::vector<QPointF> points;
        if (part.size() < 2)
            throw std::invalid_argument("invalid part.. points is smaller than two");

        QPointF p1(part[0].x, part[0].y);
        for (auto it = part.begin() + 1; it != part.end(); ++it) {
            QPointF p2(it->x, it->y);
            painter.drawLine(p1, p2);
            p1 = std::move(p2);
        }
    }
}

void NodeGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //std::cout << "paint called on division: " << this->_division_id << std::endl;
    Q_UNUSED(widget);
    painter->setClipRect(option->exposedRect);
    const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    QPen linkPen = painter->pen();
    QPen nodePen = linkPen;

    linkPen.setWidth(getPenWidth(lod));
    nodePen.setColor(Qt::red);
    nodePen.setWidth(getPenWidth(lod));
    painter->setPen(nodePen);
    // draw nodes

    size_t inc = 1;
    if (lod < getLevelOfDetailsBound(LevelOfDetails::VERY_FAR_AWAY)) //当缩放时，需要用很细的线
        inc = 5;
    else if (lod < getLevelOfDetailsBound(LevelOfDetails::FAR_AWAY))
        inc = 3;

    auto &division_node_vec = _mapdata->adj_map.divided_map[this->_division_id].second;
    for (size_t i = 0; i < division_node_vec.size(); i += inc)
        this->drawNode(*division_node_vec[i], *painter, lod);

    painter->setPen(linkPen);

    auto &division_link_vec = _mapdata->adj_map.divided_map[this->_division_id].first;
    for (size_t i = 0; i < division_link_vec.size(); i += inc)
        this->drawLink(*division_link_vec[i], *painter, lod);

    drawWay(*painter, lod);
}

NodeId NodeGraphicsItem::findNearestNode(QPointF &p)
{
    // select the current click to start node
    double x = p.rx();
    double y = p.ry();

    NodeId nodeId;
    double minDelta = -1;
    for (const auto &node : _mapdata->adj_map.node_map) {
        double px = node.second->point.x;
        double py = node.second->point.y;
        double delta = std::sqrt(std::pow(px - x, 2) + std::pow(py - y, 2));
        if (delta < minDelta || minDelta == -1) {
            minDelta = delta;
            nodeId = node.first;
        }
    }

    return nodeId;
}

void NodeGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        std::cout << "mouse left press event: ("
            << event->pos().rx() << ", " << event->pos().ry() << ")" << std::endl;
        switch (_sel_node_state) {
        case SelectNodeState::SEL_START:
        {
            // select the current click to start node
            NodeId nodeId = findNearestNode(event->pos());
            Row row = _mapdata->node_loader.getRow(_mapdata->adj_map.node_map[nodeId]->index);
            std::string start_node_name = row.strings["NODE_NAME"];
            std::cout << "selected start node id: " << nodeId << " name: " << start_node_name << std::endl;
            _start = nodeId;
            this->update();
            _sel_node_state = SelectNodeState::SEL_END;

            QMessageBox msgBox;
            msgBox.setText(QString() + "Start point selected("
                + QString::fromLocal8Bit(start_node_name.c_str())
                + "[" + QString::number(_start) + "])");
            msgBox.exec();

            break;
        }

        case SelectNodeState::SEL_END:
        {
            // select the current click to end node
            NodeId nodeId = findNearestNode(event->pos());
            Row row = _mapdata->node_loader.getRow(_mapdata->adj_map.node_map[nodeId]->index);
            std::string end_node_name = row.strings["NODE_NAME"];
            std::cout << "selected end node id: " << nodeId << " name: " << end_node_name << std::endl;
            _end = nodeId;

            row = _mapdata->node_loader.getRow(_mapdata->adj_map.node_map[_start]->index);
            std::string start_node_name = row.strings["NODE_NAME"];

            QMessageBox msgBox;
            msgBox.setText(QString() + "Do you want to find the way?");
            msgBox.setInformativeText(QString() + "From : "
                + QString::fromLocal8Bit(start_node_name.c_str()) + "[" + QString::number(_start) + "]\nTo : "
                + QString::fromLocal8Bit(end_node_name.c_str()) + "[" + QString::number(_end) + "]");
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Ok);
            if (msgBox.exec() == QMessageBox::Cancel) {
                _sel_node_state = SelectNodeState::SEL_START;
                break;
            }

            try {
                Graph::Way way = Graph::get_shortest_path(_mapdata->adj_map, _start, nodeId);
                Graph::print(way);
                _way = std::move(way);
                this->update();

                QMessageBox msgBox;
                msgBox.setText(QString() + "Way found!");
                msgBox.setInformativeText(QString() + "From : "
                    + QString::fromLocal8Bit(start_node_name.c_str()) + "[" + QString::number(_start) + "]\nTo : "
                    + QString::fromLocal8Bit(end_node_name.c_str()) + "[" + QString::number(_end) + "]");
                msgBox.exec();
                _sel_node_state = SelectNodeState::WAY_DRAWN;
            } catch (const std::invalid_argument &e) {
                QMessageBox msgBox;
                msgBox.setText(QString() + "No way found!");
                msgBox.setInformativeText(QString() + "From : "
                    + QString::fromLocal8Bit(start_node_name.c_str()) + "[" + QString::number(_start) + "]\nTo : "
                    + QString::fromLocal8Bit(end_node_name.c_str()) + "[" + QString::number(_end) + "]");
                msgBox.exec();
                _sel_node_state = SelectNodeState::SEL_START;
            }

            break;
        }

        case SelectNodeState::WAY_DRAWN:
        {
            this->_way.clear();
            this->update();
            _sel_node_state = SelectNodeState::SEL_START;
            break;
        }

        default: // Unknown state... let's just refresh!
            _sel_node_state = SelectNodeState::SEL_START;
            break;
        }
    }
}
