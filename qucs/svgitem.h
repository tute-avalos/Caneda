/***************************************************************************
 * Copyright (C) 2007 by Gopala Krishna A <krishna.ggk@gmail.com>          *
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

#ifndef __SVGITEM_H
#define __SVGITEM_H

#include "item.h"

#include <QtSvg/QSvgRenderer>
#include <QtCore/QHash>

/* Forward declarations */
class SvgPainter;

/*! \brief This class packs some information needed by svg items which are
 *         shared by many items.
 */
class SvgItemData
{
   public:
      SvgItemData(const QString& _groupId, const QByteArray& _content);

      void setStyleSheet(const QByteArray& stylesheet);
      QByteArray styleSheet() const;

      //! Returns the bounding rect of the svg element with group id = \a groupId.
      QRectF boundingRect() const { return renderer.boundsOnElement(groupId); }

   private:
      friend class SvgPainter;

      const QString groupId; //!< Represents group id of svg element.
      QByteArray content; //!< Represents raw svg content.
      qreal cachedStrokeWidth; //!< Represents stroke width , which is cached.
      QSvgRenderer renderer; //!< Represents svg renderer which renders svg.
      bool pixmapDirty; //!< Takes care of dirtyness of the pixmap cache.
};

//! A typedef for string a hash table with string keys and pointer to svg data.
typedef QHash<QString, SvgItemData*> DataHash;

/*!\brief A class used to take care of rendering svg.
 *
 * This class renders an svg given the group id. The svg to be rendered should
 * be first registered with the instance of this class. This class also
 * supports modifying style of svg using css.
 * \sa registerSvg()
 */
struct SvgPainter : public QObject
{
      Q_OBJECT;
   public:
      SvgPainter();
      ~SvgPainter();

      void registerSvg(const QString& _groupId, const QByteArray& content);
      /*! Returns whether the svg corresponding to \_groupId is registered
       * or not with \a registerSvg.
       */
      bool isSvgRegistered(const QString& _groupId) const {
         return m_dataHash.contains(_groupId);
      }

      QSvgRenderer *rendererFor(const QString& _groupId) const;
      QRectF boundingRect(const QString& _groupId) const;

      void paint(QPainter *painter, const QString& _groupId);
      SvgItemData* svgData(const QString& _groupId) const;

      QByteArray svgContent(const QString& _groupId) const;
      qreal strokeWidth(const QString& _groupId) const;

      //! Returns whether caching is enabled for the svgs or not.
      bool isCachingEnabled() const { return m_cachingEnabled; }
      void setCachingEnabled(bool caching);

      void setStyleSheet(const QString& _groupId, const QByteArray& stylesheet);
      QByteArray styleSheet(const QString& _groupId) const;

   private:
      QHash<QString, SvgItemData*> m_dataHash; //!< Holds svg data in a hash table.
      bool m_cachingEnabled; //!< State to hold whether caching is enabled or not.
};

/*!\brief Base class for items which can be rendered using svg.
 *
 * This class implements an interface needed by SvgPainter to render the svg.
 * To use the svg registered to \a SvgPainter the connection's of this item
 * to SvgPainter should be made using \a registerConnections.
 * \sa SvgPainter, registerConnections()
 */
class SvgItem : public QObject, public QucsItem
{
      Q_OBJECT;
   public:
      //! Item identifier. \sa QucsItemTypes
      enum {
         Type = QucsItemType+1
      };

      explicit SvgItem(SchematicScene *scene = 0);
      ~SvgItem();

      //! Item identifier.
      int type() const { return SvgItem::Type; }

      void paint(QPainter *p, const QStyleOptionGraphicsItem* o, QWidget *w);

      qreal strokeWidth() const;

      void registerConnections(const QString& id, SvgPainter *painter);
      //! Returns the svg group id of this item.
      QString groupId() const { return m_groupId; }

      QByteArray svgContent() const;
      /*! \brief Returns the \a SvgPainter to which the item is connected to.
       * Returns null if it isn't registered.
       */
      SvgPainter* svgPainter() const { return m_svgPainter; }

   public slots:
      void updateBoundingRect();

   protected:
      /*! \brief This virtual method is used to tackle special condition where the
       * the rect of this item has to be bigger than the actual svg rect.
       * Reimplement in derived classes to adjust accordingly.
       */
      virtual QRectF adjustedBoundRect(const QRectF &rect) { return rect; }
      QVariant itemChange(GraphicsItemChange change, const QVariant& value);

   private:
      SvgPainter *m_svgPainter;
      QString m_groupId;
};

#endif //__SVGITEM_H
