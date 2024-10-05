#ifndef HIGHLIGHTRULEFORM_HPP
#define HIGHLIGHTRULEFORM_HPP

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QColorDialog>
#include <QColor>
#include <QRegularExpression>
#include <QLabel>
#include <QHBoxLayout>

#include "HighlightRule.hpp"

class HighlightRuleForm : public QDialog {
    Q_OBJECT
public:
    HighlightRuleForm(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle(tr("Highlight"));

        // Layout do formulário
        QFormLayout *layout = new QFormLayout(this);

        // Campo para expressão regular
        regexEdit = new QLineEdit(this);
        layout->addRow(tr("Regex:"), regexEdit);

        // CheckBox para usar cor do texto com quadradinho de preview
        QHBoxLayout *foregroundLayout = new QHBoxLayout();
        useForegroundColorCheckBox = new QCheckBox(this);
        foregroundLayout->addWidget(useForegroundColorCheckBox);
        foregroundColorPreview = new QLabel(this);
        foregroundColorPreview->setFixedSize(20, 20);
        foregroundColorPreview->setAutoFillBackground(true);
        foregroundColorPreview->setStyleSheet("border: 1px solid black");
        foregroundLayout->addWidget(foregroundColorPreview);
        layout->addRow(tr("Use foreground color:"), foregroundLayout);

        // Botão para selecionar a cor do texto
        selectForegroundColorButton = new QPushButton(tr("Change foreground color"), this);
        layout->addRow(selectForegroundColorButton);
        connect(selectForegroundColorButton, &QPushButton::clicked, this, &HighlightRuleForm::selectForegroundColor);

        // CheckBox para usar cor de fundo com quadradinho de preview
        QHBoxLayout *backgroundLayout = new QHBoxLayout();
        useBackgroundColorCheckBox = new QCheckBox(this);
        backgroundLayout->addWidget(useBackgroundColorCheckBox);
        backgroundColorPreview = new QLabel(this);
        backgroundColorPreview->setFixedSize(20, 20);
        backgroundColorPreview->setAutoFillBackground(true);
        backgroundColorPreview->setStyleSheet("border: 1px solid black");
        backgroundLayout->addWidget(backgroundColorPreview);
        layout->addRow(tr("Use background color:"), backgroundLayout);

        // Botão para selecionar a cor de fundo
        selectBackgroundColorButton = new QPushButton(tr("Change background color"), this);
        layout->addRow(selectBackgroundColorButton);
        connect(selectBackgroundColorButton, &QPushButton::clicked, this, &HighlightRuleForm::selectBackgroundColor);

        // Preview de como ficará o texto com as cores
        previewLabel = new QLabel(tr("Highlighted text"), this);
        layout->addRow(tr("Preview:"), previewLabel);

        // Botões Save e Cancel
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *saveButton = new QPushButton(tr("Save"), this);
        QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
        buttonLayout->addWidget(saveButton);
        buttonLayout->addWidget(cancelButton);
        layout->addRow(buttonLayout);

        connect(saveButton, &QPushButton::clicked, this, &HighlightRuleForm::saveRule);
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    }

signals:
    void ruleSaved(const HighlightRule &rule);

public:
    HighlightRule getHighlightRule() const {
        return rule;
    }

public slots:
    void setRule(const HighlightRule &rule) {
        regexEdit->setText(rule.regex.pattern());
        useForegroundColorCheckBox->setChecked(rule.useForegroundColor);
        useBackgroundColorCheckBox->setChecked(rule.useBackgroundColor);
        updateForeground(rule.foregroundColor);
        updateBackground(rule.backgroundColor);
        updatePreview();
    }

    void disableRegexEdit() {
        regexEdit->setDisabled(true);
    }


private slots:
    void selectForegroundColor() {
        QColor color = QColorDialog::getColor(Qt::white, this, tr("Foreground"));
        if (color.isValid()) {
            updateForeground(color);
            updatePreview();
        }
    }

    void selectBackgroundColor() {
        QColor color = QColorDialog::getColor(Qt::white, this, tr("Background"));
        if (color.isValid()) {
            updateBackground(color);
            updatePreview();
        }
    }

    void updatePreview() {
        QString style;
        if (useForegroundColorCheckBox->isChecked()) {
            style += QString("color: %1; ").arg(rule.foregroundColor.name());
        }
        if (useBackgroundColorCheckBox->isChecked()) {
            style += QString("background-color: %1; ").arg(rule.backgroundColor.name());
        }
        previewLabel->setStyleSheet(style);
    }

    void saveRule() {
        rule.regex = QRegularExpression(regexEdit->text());
        rule.useForegroundColor = useForegroundColorCheckBox->isChecked();
        rule.useBackgroundColor = useBackgroundColorCheckBox->isChecked();
        emit ruleSaved(rule); // Emite o sinal ao salvar
        accept(); // Fecha o diálogo modal
    }

private:
    void updateForeground(QColor color) {
        rule.foregroundColor = color;
        foregroundColorPreview->setStyleSheet(QString("border: 1px solid black; background-color: %1").arg(color.name()));
    }

    void updateBackground(QColor color) {
        rule.backgroundColor = color;
        backgroundColorPreview->setStyleSheet(QString("border: 1px solid black; background-color: %1").arg(color.name()));
    }

private:
    HighlightRule rule;
    QLineEdit *regexEdit;
    QCheckBox *useForegroundColorCheckBox;
    QLabel *foregroundColorPreview;
    QPushButton *selectForegroundColorButton;
    QCheckBox *useBackgroundColorCheckBox;
    QLabel *backgroundColorPreview;
    QPushButton *selectBackgroundColorButton;
    QLabel *previewLabel;
};

#endif // HIGHLIGHTRULEFORM_HPP
