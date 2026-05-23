#pragma once

#include <QIcon>
#include <QColor>
#include <QString>

namespace keyoubs {

class PhosphorIcons
{
public:
    static QIcon icon(const QString &name,
                      const QColor &color = QColor(0x62, 0xA9, 0xE3),
                      int size = 20);

    // Pre-defined icon names matching our resource paths
    static constexpr const char *Cursor                 = "cursor";
    static constexpr const char *AppWindow              = "app-window";
    static constexpr const char *Layout                 = "layout";
    static constexpr const char *ArrowsOutLineHorizontal= "arrows-out-line-horizontal";
    static constexpr const char *FrameCorners           = "frame-corners";
    static constexpr const char *Check                  = "check";
    static constexpr const char *ArrowCounterClockwise  = "arrow-counter-clockwise";
    static constexpr const char *Cross                  = "cross";
    static constexpr const char *FloppyDisk             = "floppy-disk";
    static constexpr const char *Trash                  = "trash";
    static constexpr const char *SquaresFour            = "squares-four";
};

} // namespace keyoubs
