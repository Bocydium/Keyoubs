#include "PhosphorIcons.hpp"

#include <QSvgRenderer>
#include <QPainter>
#include <QFile>
#include <QPixmap>

namespace keyoubs {

QIcon PhosphorIcons::icon(const QString &name, const QColor &color, int size)
{
    QString path = QString(":/icons/icons/phosphor/%1.svg").arg(name);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QIcon();
    }

    QByteArray svgData = file.readAll();
    QString svgText = QString::fromUtf8(svgData);
    svgText.replace("fill=\"currentColor\"", QString("fill=\"%1\"").arg(color.name()));

    QSvgRenderer renderer(svgText.toUtf8());
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    renderer.render(&painter);
    painter.end();

    return QIcon(pixmap);
}

} // namespace keyoubs
