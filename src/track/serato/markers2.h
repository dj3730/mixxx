#pragma once

#include <QByteArray>
#include <QColor>
#include <QDataStream>
#include <QList>
#include <memory>

#include "util/color/rgbcolor.h"
#include "util/types.h"

namespace {
constexpr mixxx::RgbColor kDefaultTrackColor = mixxx::RgbColor(0xFF9999);
constexpr mixxx::RgbColor kDefaultCueColor = mixxx::RgbColor(0xCC0000);
} // namespace

namespace mixxx {

// Enum values need to appear in the same order as the corresponding entries
// are written to the tag by Serato.
class SeratoMarkers2Entry {
  public:
    enum class TypeId {
        Unknown,
        Color,
        Cue,
        Loop,
        Bpmlock,
    };

    virtual ~SeratoMarkers2Entry() = default;

    virtual QString type() const = 0;
    virtual SeratoMarkers2Entry::TypeId typeId() const = 0;

    virtual QByteArray dump() const = 0;

    virtual quint32 length() const = 0;
};

typedef std::shared_ptr<SeratoMarkers2Entry> SeratoMarkers2EntryPointer;

inline bool operator==(const SeratoMarkers2Entry& lhs, const SeratoMarkers2Entry& rhs) {
    return (lhs.type() == rhs.type()) && (lhs.dump() == rhs.dump());
}

inline bool operator!=(const SeratoMarkers2Entry& lhs, const SeratoMarkers2Entry& rhs) {
    return !(lhs == rhs);
}

inline QDebug operator<<(QDebug dbg, const SeratoMarkers2Entry& arg) {
    return dbg << "type =" << arg.type()
               << "data =" << arg.dump()
               << "("
               << "length =" << arg.length() << ")";
}

class SeratoMarkers2UnknownEntry : public SeratoMarkers2Entry {
  public:
    SeratoMarkers2UnknownEntry(QString type, QByteArray data)
            : m_type(std::move(type)),
              m_data(std::move(data)) {
    }
    ~SeratoMarkers2UnknownEntry() override = default;

    QString type() const override {
        return m_type;
    }

    SeratoMarkers2Entry::TypeId typeId() const override {
        return SeratoMarkers2Entry::TypeId::Unknown;
    }

    QByteArray dump() const override {
        return m_data;
    }

    quint32 length() const override {
        return dump().length();
    }

  private:
    QString m_type;
    QByteArray m_data;
};

class SeratoMarkers2BpmlockEntry : public SeratoMarkers2Entry {
  public:
    SeratoMarkers2BpmlockEntry(bool locked)
            : m_locked(locked) {
    }

    SeratoMarkers2BpmlockEntry()
            : m_locked(false) {
    }

    static SeratoMarkers2EntryPointer parse(const QByteArray& data);

    QString type() const override {
        return "BPMLOCK";
    }

    SeratoMarkers2Entry::TypeId typeId() const override {
        return SeratoMarkers2Entry::TypeId::Bpmlock;
    }

    QByteArray dump() const override;

    bool isLocked() const {
        return m_locked;
    }

    void setLocked(bool locked) {
        m_locked = locked;
    }

    quint32 length() const override;

  private:
    bool m_locked;
};

inline bool operator==(const SeratoMarkers2BpmlockEntry& lhs,
        const SeratoMarkers2BpmlockEntry& rhs) {
    return (lhs.isLocked() == rhs.isLocked());
}

inline bool operator!=(const SeratoMarkers2BpmlockEntry& lhs,
        const SeratoMarkers2BpmlockEntry& rhs) {
    return !(lhs == rhs);
}

inline QDebug operator<<(QDebug dbg, const SeratoMarkers2BpmlockEntry& arg) {
    return dbg << "locked =" << arg.isLocked();
}

class SeratoMarkers2ColorEntry : public SeratoMarkers2Entry {
  public:
    SeratoMarkers2ColorEntry(RgbColor color)
            : m_color(color) {
    }

    SeratoMarkers2ColorEntry()
            : m_color(kDefaultTrackColor) {
    }

    static SeratoMarkers2EntryPointer parse(const QByteArray& data);

    QString type() const override {
        return "COLOR";
    }

    SeratoMarkers2Entry::TypeId typeId() const override {
        return SeratoMarkers2Entry::TypeId::Color;
    }

    QByteArray dump() const override;

    RgbColor getColor() const {
        return m_color;
    }

    void setColor(RgbColor color) {
        m_color = color;
    }

    quint32 length() const override;

  private:
    RgbColor m_color;
};

inline bool operator==(const SeratoMarkers2ColorEntry& lhs,
        const SeratoMarkers2ColorEntry& rhs) {
    return (lhs.getColor() == rhs.getColor());
}

inline bool operator!=(const SeratoMarkers2ColorEntry& lhs,
        const SeratoMarkers2ColorEntry& rhs) {
    return !(lhs == rhs);
}

inline QDebug operator<<(QDebug dbg, const SeratoMarkers2ColorEntry& arg) {
    return dbg << "color =" << arg.getColor();
}

class SeratoMarkers2CueEntry : public SeratoMarkers2Entry {
  public:
    SeratoMarkers2CueEntry(quint8 index, quint32 position, RgbColor color, QString label)
            : m_index(index),
              m_position(position),
              m_color(color),
              m_label(label) {
    }

    SeratoMarkers2CueEntry()
            : m_index(0),
              m_position(0),
              m_color(kDefaultCueColor),
              m_label(QString("")) {
    }

    static SeratoMarkers2EntryPointer parse(const QByteArray& data);

    QString type() const override {
        return "CUE";
    }

    SeratoMarkers2Entry::TypeId typeId() const override {
        return SeratoMarkers2Entry::TypeId::Cue;
    }

    QByteArray dump() const override;

    quint8 getIndex() const {
        return m_index;
    }

    void setIndex(quint8 index) {
        m_index = index;
    }

    quint32 getPosition() const {
        return m_position;
    }

    void setPosition(quint32 position) {
        m_position = position;
    }

    RgbColor getColor() const {
        return m_color;
    }

    void setColor(RgbColor color) {
        m_color = color;
    }

    QString getLabel() const {
        return m_label;
    }

    void setLabel(QString label) {
        m_label = label;
    }

    quint32 length() const override;

  private:
    quint8 m_index;
    quint32 m_position;
    RgbColor m_color;
    QString m_label;
};

inline bool operator==(const SeratoMarkers2CueEntry& lhs,
        const SeratoMarkers2CueEntry& rhs) {
    return (lhs.getIndex() == rhs.getIndex()) &&
            (lhs.getPosition() == rhs.getPosition()) &&
            (lhs.getColor() == rhs.getColor()) &&
            (lhs.getLabel() == rhs.getLabel());
}

inline bool operator!=(const SeratoMarkers2CueEntry& lhs,
        const SeratoMarkers2CueEntry& rhs) {
    return !(lhs == rhs);
}

inline QDebug operator<<(QDebug dbg, const SeratoMarkers2CueEntry& arg) {
    return dbg << "index =" << arg.getIndex() << "/"
               << "position =" << arg.getPosition() << "/"
               << "color =" << arg.getColor() << "/"
               << "label =" << arg.getLabel();
}

class SeratoMarkers2LoopEntry : public SeratoMarkers2Entry {
  public:
    SeratoMarkers2LoopEntry(quint8 index, quint32 startposition, quint32 endposition, bool locked, QString label)
            : m_index(index),
              m_startposition(startposition),
              m_endposition(endposition),
              m_locked(locked),
              m_label(label) {
    }

    SeratoMarkers2LoopEntry()
            : m_index(0),
              m_startposition(0),
              m_endposition(0),
              m_locked(false),
              m_label(QString("")) {
    }

    static SeratoMarkers2EntryPointer parse(const QByteArray& data);

    QString type() const override {
        return "LOOP";
    }

    SeratoMarkers2Entry::TypeId typeId() const override {
        return SeratoMarkers2Entry::TypeId::Loop;
    }

    QByteArray dump() const override;

    quint8 getIndex() const {
        return m_index;
    }

    void setIndex(quint8 index) {
        m_index = index;
    }

    quint32 getStartPosition() const {
        return m_startposition;
    }

    void setStartPosition(quint32 startposition) {
        m_startposition = startposition;
    }

    quint32 getEndPosition() const {
        return m_endposition;
    }

    void setEndPosition(quint32 endposition) {
        m_endposition = endposition;
    }

    bool isLocked() const {
        return m_locked;
    }

    void setLocked(bool locked) {
        m_locked = locked;
    }

    QString getLabel() const {
        return m_label;
    }

    void setLabel(QString label) {
        m_label = label;
    }

    quint32 length() const override;

  private:
    quint8 m_index;
    quint32 m_startposition;
    quint32 m_endposition;
    bool m_locked;
    QString m_label;
};

inline bool operator==(const SeratoMarkers2LoopEntry& lhs,
        const SeratoMarkers2LoopEntry& rhs) {
    return (lhs.getIndex() == rhs.getIndex()) &&
            (lhs.getStartPosition() == rhs.getStartPosition()) &&
            (lhs.getEndPosition() == rhs.getEndPosition()) &&
            (lhs.isLocked() == rhs.isLocked()) &&
            (lhs.getLabel() == rhs.getLabel());
}

inline bool operator!=(const SeratoMarkers2LoopEntry& lhs,
        const SeratoMarkers2LoopEntry& rhs) {
    return !(lhs == rhs);
}

inline QDebug operator<<(QDebug dbg, const SeratoMarkers2LoopEntry& arg) {
    return dbg << "index =" << arg.getIndex() << "/"
               << "startposition =" << arg.getStartPosition() << "/"
               << "endposition =" << arg.getEndPosition() << "/"
               << "locked =" << arg.isLocked() << "/"
               << "label =" << arg.getLabel();
}

// DTO for storing information from the SeratoMarkers2 tags used by the Serato
// DJ Pro software.
//
// Parsing & Formatting
// --------------------
// This class includes functions for formatting and parsing SeratoMarkers2
// metadata according to the specification:
// https://github.com/Holzhaus/serato-tags/blob/master/docs/serato_markers2.md
//
class SeratoMarkers2 final {
  public:
    SeratoMarkers2() = default;
    explicit SeratoMarkers2(
            QList<std::shared_ptr<SeratoMarkers2Entry>> entries)
            : m_allocatedSize(0),
              m_entries(std::move(entries)) {
    }

    // Parsing and formatting of gain values according to the
    // SeratoMarkers2 1.0/2.0 specification.
    static bool parse(SeratoMarkers2* seratoMarkers2, const QByteArray& outerData);

    QByteArray dump() const;

    int getAllocatedSize() const {
        return m_allocatedSize;
    }

    void setAllocatedSize(int size) {
        DEBUG_ASSERT(size >= 0);
        m_allocatedSize = size;
    }

    bool isEmpty() const {
        return m_entries.isEmpty();
    }

    const QList<std::shared_ptr<SeratoMarkers2Entry>>& getEntries() const {
        return m_entries;
    }
    void setEntries(QList<std::shared_ptr<SeratoMarkers2Entry>> entries) {
        m_entries = std::move(entries);
    }

  private:
    int m_allocatedSize;
    QList<std::shared_ptr<SeratoMarkers2Entry>> m_entries;
};

inline bool operator==(const SeratoMarkers2& lhs, const SeratoMarkers2& rhs) {
    return (lhs.getEntries() == rhs.getEntries());
}

inline bool operator!=(const SeratoMarkers2& lhs, const SeratoMarkers2& rhs) {
    return !(lhs == rhs);
}

inline QDebug operator<<(QDebug dbg, const SeratoMarkers2& arg) {
    return dbg << "entries =" << arg.getEntries().length();
}

} // namespace mixxx

Q_DECLARE_TYPEINFO(mixxx::SeratoMarkers2, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(mixxx::SeratoMarkers2)
