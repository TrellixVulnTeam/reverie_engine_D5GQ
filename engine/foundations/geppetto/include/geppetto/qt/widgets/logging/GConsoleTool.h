#ifndef GB_CONSOLE_TOOL
#define GB_CONSOLE_TOOL

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard Includes

// External
#include <QtWidgets>

// Internal
#include "fortress/types/GNameable.h"
#include "logging/GLogger.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace rev {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Override QTextEdit so that cursor can't be moved
class ReadOnlyTextEdit: public QTextEdit {
public:
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///	@brief  Displays Logger output to a window.
///    @note Can be styled with stylesheet

class ConsoleTool : public QWidget, public rev::NameableInterface, public AbstractLogHandler{
	Q_OBJECT
public:

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Constructors and Destructors
    /// @{
    ConsoleTool(const QString& name, QWidget *parent = 0);
    virtual ~ConsoleTool() = default;
    /// @}
	//-----------------------------------------------------------------------------------------------------------------
	/// @name Properties
	/// @{
	/** @brief Color of Debug text
	    @details This value can be styled via QStyleSheet as follows:
            @code {.unparsed}
            rev--View--ConsoleTool {qproperty-foregroundColorDebug: yellow;}
            @endcode
	*/
    Q_PROPERTY(QColor foregroundColorDebug READ foregroundColorDebug WRITE setForegroundColorDebug DESIGNABLE true)
	inline const QColor& foregroundColorDebug() const { return m_foreground_color_debug; }
	void setForegroundColorDebug(const QColor& fg_color) { m_foreground_color_debug = fg_color; }

    /** @brief Color of Error text
	    @details This value can be styled via QStyleSheet as follows:
            @code {.unparsed}
             rev--View--ConsoleTool {qproperty-foregroundColorError: red;}
            @endcode
	*/
    Q_PROPERTY(QColor foregroundColorError READ foregroundColorError WRITE setForegroundColorError DESIGNABLE true)
	inline const QColor& foregroundColorError() const { return m_foreground_color_error; }
	void setForegroundColorError(const QColor& fg_color) { m_foreground_color_error = fg_color; }

    /** @brief Color of Info text
	    @details This value can be styled via QStyleSheet as follows:
            @code {.unparsed}
             rev--View--ConsoleTool {qproperty-foregroundColorInfo: yellow;}
            @endcode
	*/
    Q_PROPERTY(QColor foregroundColorInfo READ foregroundColorInfo WRITE setForegroundColorInfo DESIGNABLE true)
	inline const QColor& foregroundColorInfo() const { return m_foreground_color_info; }
	void setForegroundColorInfo(const QColor& fg_color) { m_foreground_color_info = fg_color; }

    /** @brief Color of Warning text
	    @details This value can be styled via QStyleSheet as follows:
            @code {.unparsed}
             rev--View--ConsoleTool {qproperty-foregroundColorWarning: #0080ff;}
            @endcode
	*/
    Q_PROPERTY(QColor foregroundColorWarning READ foregroundColorWarning WRITE setForegroundColorWarning DESIGNABLE true)
	inline const QColor& foregroundColorWarning() const { return m_foreground_color_warning; }
	void setForegroundColorWarning(const QColor& fg_color) { m_foreground_color_warning = fg_color; }

	/// @}
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Public Methods
    /// @{
	/** @brief Output log messages to console
	    @param[in] log_record The data for the log to be output.
	*/
	void output(rev::LogRecord& log_record) override;

    /// @brief Show the console tool when added to the logger
    virtual void onAddToLogger() override;

    /// @}

signals:
    void outputRecordPvt(const rev::LogRecord& logRecord);

private slots:
    void handleOutputRecord(const rev::LogRecord& logRecord);

private:
    /// @brief Scroll to the bottom of the console window
    void scrollToBottom();

    QMutex m_logMutex;
    QColor m_foreground_color_debug;
	QColor m_foreground_color_error;
	QColor m_foreground_color_warning;
	QColor m_foreground_color_info;
    ReadOnlyTextEdit* m_consoleTextEdit;
    LogRecord m_mostRecentLogRecord;
};
				
} // rev

#endif 