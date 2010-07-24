#include "decorationstyleloader.hpp"

#include <QResource>
#include <QDir>
#include <QSettings>
#include <QDebug>

class qResource: public QResource
{
public:
    qResource(const QString &path = QString())
        : QResource(path)
    {
    }

    inline QStringList childrenList() { return children(); }
};

DecorationStyleLoader::DecorationStyleLoader(const QString &file)
    : _valid(true)
{
    qResource oldResource("/styles/");
    QStringList oldList = oldResource.childrenList();

    if (!QResource::registerResource(file))
    {
        _valid = false;
        return;
    }

    qResource rootResource("/styles/");
    if (!rootResource.isValid() || oldList.size() == rootResource.childrenList().size())
    {
        _valid = false;
        return;
    }

    qResource styleResource(QString("/styles/%1/").arg(rootResource.childrenList().last()));
    if (!styleResource.childrenList().contains("info.ini") ||
        !styleResource.childrenList().contains("style.qss"))
    {
        _valid = false;
        return;
    }

    if (!loadInfo(styleResource.absoluteFilePath() + "info.ini"))
    {
        _valid = false;
        return;
    }

    _styleRootPath = styleResource.absoluteFilePath();
}

DecorationStyleLoader:: DecorationStyleLoader(const DecorationStyleLoader &other)
    : _valid(other._valid)
    , _styleRootPath(other._styleRootPath)
    , _authorName(other._authorName)
    , _authorMail(other._authorMail)
    , _styleName(other._styleName)
{
}

const QString DecorationStyleLoader::styleSheet() const
{
    QFile file(_styleRootPath + "style.qss");
    file.open(QFile::ReadOnly);
    return file.readAll();
}

QList<DecorationStyleLoader> DecorationStyleLoader::styles()
{
    QList<DecorationStyleLoader> list;

    QDir dir(QDir::homePath() + "/.antico/styles/");
    foreach (const QFileInfo &styleInfo, dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot))
    {
        DecorationStyleLoader loader(styleInfo.absoluteFilePath());
        if (loader.isValid())
            list.append(loader);
    }

    return list;
}

bool DecorationStyleLoader::loadInfo(const QString &path)
{
    QSettings settings(path, QSettings::IniFormat);
    if (settings.status() != QSettings::NoError)
        return false;

    _authorName = settings.value("Author/Name").toString();
    _authorMail = settings.value("Author/Mail").toString();
    _styleName = settings.value("Style/Name").toString();
}
