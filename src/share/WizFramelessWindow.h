﻿#ifndef WIZFRAMELESSWINDOW_H
#define WIZFRAMELESSWINDOW_H

#include <QWidget>
#include <QPainter>
#include <QPaintEngine>
#include <QMouseEvent>
#include <QDebug>
#include <QBoxLayout>
#include <QTimer>
#include <QToolButton>

#include "WizWindowTitleBar.h"
#include "WizShadowEffect.h"

#include "libs/3rdparty/framelesshelper/src/core/framelesshelper.h"
#include "libs/3rdparty/framelesshelper/src/core/utilities.h"

template <class Base>
class WizFramelessWindow : public Base
{
public:
    explicit WizFramelessWindow(QWidget *parent, bool canResize)
        : Base(parent)
        , m_clientWidget(NULL)
        , m_clientLayout(NULL)
        , m_canResize(canResize)
        , m_helper(new __flh_ns::FramelessHelper)
    {
        this->setAttribute(Qt::WA_DontCreateNativeAncestors);

        m_titleBar = new WizWindowTitleBar(this, this, canResize);

        m_clientWidget = new QWidget(this);
        m_clientLayout = new QBoxLayout(QBoxLayout::TopToBottom);
        m_clientWidget->setLayout(m_clientLayout);
        m_clientLayout->setSpacing(0);
        m_clientLayout->setContentsMargins(0, 0, 0, 0);
    
        m_clientLayout->addWidget(m_titleBar);
    }

public:
    QWidget* rootWidget() const { return m_clientWidget; }
    QWidget *clientWidget() const { return m_clientWidget; }
    QLayout* clientLayout() const { return m_clientLayout; }
    WizWindowTitleBar* titleBar() const { return m_titleBar; }
    void setTitleText(QString title) { m_titleBar->setText(title); }

    void setHitTestVisible(QObject *obj)
    {
        m_helper->setHitTestVisible(obj);
    }

private:
    QWidget* m_clientWidget;
    QLayout* m_clientLayout;
    WizWindowTitleBar* m_titleBar;
    __flh_ns::FramelessHelper *m_helper;
    bool m_canResize;
    bool m_flhInited = false;

protected:
    void changeEvent(QEvent * event) override
    {
#ifndef Q_OS_MAC
        bool shouldUpdate = false;
        if (event->type() == QEvent::WindowStateChange) {
            if (this->isMaximized() || this->isFullScreen()) {
                this->setContentsMargins(0, 0, 0, 0);
            } else if (!this->isMinimized()) {
                this->setContentsMargins(1, 1, 1, 1);
            }
            m_titleBar->windowStateChanged();
            shouldUpdate = true;
        } else if (event->type() == QEvent::ActivationChange) {
            shouldUpdate = true;
        }
        if (shouldUpdate) {
            this->update();
        }
#endif

#ifdef Q_OS_MAC
        if (event->type() == QEvent::WindowStateChange && m_flhInited) {
            auto cev = static_cast<QWindowStateChangeEvent*>(event);
            if (cev->oldState() & Qt::WindowFullScreen && !this->isFullScreen()) {
                // Exit fullscreen
                m_titleBar->layout()->setContentsMargins(70, 0, 0, 0);
            } else if (!(cev->oldState() & Qt::WindowFullScreen) && this->isFullScreen()) {
                // Enter fullscreen
                m_titleBar->layout()->setContentsMargins(0, 0, 0, 0);
            }
        }
#endif

        Base::changeEvent(event);
    }

    virtual void layoutTitleBar()
    {
        m_titleBar->layoutTitleBar();
    }

    void showEvent(QShowEvent *event) override
    {
        Base::showEvent(event);
        if (!m_flhInited) {
            const auto win = this->windowHandle();
            if (win) {
                m_helper->setWindow(win);
                m_helper->setTitleBarHeight(m_titleBar->height());
                m_helper->setResizable(m_canResize);
                m_helper->setResizeBorderThickness(2);
                m_helper->setHitTestVisible(m_titleBar->closeButton());
                m_helper->setHitTestVisible(m_titleBar->maxButton());
                m_helper->setHitTestVisible(m_titleBar->minButton());
                m_helper->install();
#ifndef Q_OS_MAC
                this->setContentsMargins(1, 1, 1, 1);
#else
                __flh_ns::Utilities::setStandardWindowButtonsVisibility(win, true);
                QSize btnGroupSize = __flh_ns::Utilities::standardWindowButtonsSize(win);
                __flh_ns::Utilities::setStandardWindowButtonsPosition(win,
                    QPoint(12, (m_titleBar->height() - btnGroupSize.height())/2)
                );
                m_titleBar->layout()->setContentsMargins(70, 0, 0, 0);
#endif // Q_OS_MAC
                m_flhInited = true;
            }
        }
    }

#ifdef Q_OS_WIN
	bool nativeEvent(const QByteArray &eventType, void *message, long *result) override
    {
        if (!m_helper)
            return Base::nativeEvent(eventType, message, result);

        if (m_helper->handleNativeEvent(this->windowHandle(), eventType, message, result))
            return true;
        else
            return Base::nativeEvent(eventType, message, result);
    }
#endif // Q_OS_WIN
};

#endif // WIZFRAMELESSWINDOW_H