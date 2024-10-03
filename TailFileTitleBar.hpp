#ifndef TAILFILETITLEBAR_HPP
#define TAILFILETITLEBAR_HPP

#include <QMainWindow>
#include <QDockWidget>
#include <QApplication>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QStyle>
#include <QToolTip>
#include <QMouseEvent>


class TailFileTitleBar : public QWidget {
    Q_OBJECT
public:
    TailFileTitleBar(QDockWidget* dockWidget, const QString& title, const QString& tooltip, QWidget* parent = nullptr)
        : QWidget(parent), m_dockWidget(dockWidget) {
        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setContentsMargins(8, 0, 0, 0);

        // Criação do rótulo de título
        QLabel* titleLabel = new QLabel(title, this);
        layout->addWidget(titleLabel);

        // Botões de controle padrão (fechar e float)
        QToolButton* closeButton = new QToolButton(this);
        closeButton->setToolTip(tooltip);
        closeButton->setAutoRaise(true);
        closeButton->setIcon(m_dockWidget->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
        connect(closeButton, &QToolButton::clicked, m_dockWidget, &QDockWidget::close);

        // QToolButton* floatButton = new QToolButton(this);
        // floatButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarNormalButton));
        // connect(floatButton, &QToolButton::clicked, dockWidget, &QDockWidget::setFloating);

        layout->addStretch();
        // layout->addWidget(floatButton);
        layout->addWidget(closeButton);

        setLayout(layout);
    }

private:
    QDockWidget* m_dockWidget;
};


#endif // TAILFILETITLEBAR_HPP
