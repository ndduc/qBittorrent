#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QVector>

const int MAX_LOG_MESSAGES = 20000;

namespace Log
{
    enum MsgType
    {
        ALL = -1,
        NORMAL = 0x1,
        INFO = 0x2,
        WARNING = 0x4,
        CRITICAL = 0x8 // ERROR is defined by libtorrent and results in compiler error
    };
    Q_DECLARE_FLAGS(MsgTypes, MsgType)

    struct Msg
    {
        int id;
        qint64 timestamp;
        MsgType type;
        QString message;
    };

    struct Peer
    {
        int id;
        qint64 timestamp;
        QString ip;
        bool blocked;
        QString reason;
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Log::MsgTypes)

class Logger : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Logger)

public:
    Logger();
    ~Logger();

    void addMessage(const QString &message, const Log::MsgType &type = Log::NORMAL);
    void addPeer(const QString &ip, bool blocked, const QString &reason = QString());
    QVector<Log::Msg> getMessages(int lastKnownId = -1) const;
    QVector<Log::Peer> getPeers(int lastKnownId = -1) const;

signals:
    void newLogMessage(const Log::Msg &message);
    void newLogPeer(const Log::Peer &peer);

private:
    QVector<Log::Msg> m_messages;
    QVector<Log::Peer> m_peers;
    mutable QReadWriteLock m_lock;
    int m_msgCounter;
    int m_peerCounter;
};

#endif // LOGGER_H
