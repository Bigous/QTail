#ifndef FIXDICTIONARY_HPP
#define FIXDICTIONARY_HPP

#include <QMap>
#include <QObject>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QXmlStreamReader>

struct FixEnumFieldValue {
    QString enumValue;
    QString description;
};

struct FixField {
    int tag;
    QString name;
    QString type;
    QMap<QString, QString> valueDescription;
};

class FixDictionary: public QObject {
    Q_OBJECT
public:
    enum Retrieved {
        None = 0,
        TagName = 1,
        ValueDescription = 2
    };

    FixDictionary(QObject *parent = nullptr): QObject(parent) {}
    virtual ~FixDictionary() {}

    bool loadDictionary(const QString& path) {
        QFile file(path);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Failed to open FIX dictionary file";
            return false;
        }

        QXmlStreamReader reader(&file);

        FixField currentField;

        while (!reader.atEnd() && !reader.hasError()) {
            // Ler o próximo elemento
            QXmlStreamReader::TokenType token = reader.readNext();

            // Se for o início de um elemento
            if (token == QXmlStreamReader::StartElement) {
                if (reader.name().toString() == "field") {
                    currentField = FixField(); // Iniciar um novo campo

                    currentField.tag = reader.attributes().value("number").toInt();
                    currentField.name = reader.attributes().value("name").toString();
                    currentField.type = reader.attributes().value("type").toString();

                } else if (reader.name().toString() == "value") {
                    // Valores de enum, se existirem
                    QString enumValue = reader.attributes().value("enum").toString();
                    QString description = reader.attributes().value("description").toString();
                    currentField.valueDescription[enumValue] = description;

                }
            }

            // Quando um campo estiver completo, adicione-o ao mapa
            if (token == QXmlStreamReader::EndElement && reader.name().toString() == "field" && currentField.tag > 0) {
                m_fields[currentField.tag] = currentField;
            }
        }

        if (reader.hasError()) {
            qWarning() << "Error parsing FIX dictionary:" << reader.errorString();
            return false;
        }

        return true;
    }

    QString getTagName(int tag) const {
        if (m_fields.contains(tag)) {
            return m_fields[tag].name;
        }
        return QString();
    }

    QString getFieldValueDescription(int tag, const QString& value) const {
        if (m_fields.contains(tag)) {
            const FixField& field = m_fields[tag];
            if (field.valueDescription.contains(value)) {
                return field.valueDescription[value];
            }
        }
        return QString();
    }

    Retrieved TryGetTagNameAndFieldValueDescription(int tag, const QString& value, QString& tagName, QString& valueDescription) const {
        auto it = m_fields.constFind(tag);
        if (it != m_fields.constEnd()) {
            const FixField& field = *it;
            tagName = field.name;
            if (!field.valueDescription.empty()) {
                auto it2 = field.valueDescription.constFind(value);
                if(it2 != field.valueDescription.constEnd()) {
                    valueDescription = *it2;
                    return ValueDescription;
                }
            }
            return TagName;
        }
        return None;
    }

private:
    QMap<int, FixField> m_fields;
};

#endif // FIXDICTIONARY_HPP
