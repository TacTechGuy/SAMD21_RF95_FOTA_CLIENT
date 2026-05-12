<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(Form1))
        Me.FolderBrowserDialog1 = New System.Windows.Forms.FolderBrowserDialog()
        Me.PictureBox1 = New System.Windows.Forms.PictureBox()
        Me.btnScanPorts = New System.Windows.Forms.Button()
        Me.btnWriteCommand = New System.Windows.Forms.Button()
        Me.btnConnect = New System.Windows.Forms.Button()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.cmbSerialPort = New System.Windows.Forms.ComboBox()
        Me.cmbBaud = New System.Windows.Forms.ComboBox()
        Me.ListBox1 = New System.Windows.Forms.ListBox()
        Me.lblDate = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.lblTime = New System.Windows.Forms.Label()
        Me.SerialPort1 = New System.IO.Ports.SerialPort(Me.components)
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.lblStreamData = New System.Windows.Forms.Label()
        Me.lblTest = New System.Windows.Forms.Label()
        Me.txtWriteCommand = New System.Windows.Forms.TextBox()
        Me.fileOpen = New System.Windows.Forms.OpenFileDialog()
        Me.btnOpen = New System.Windows.Forms.Button()
        Me.btnSendBinaryFile = New System.Windows.Forms.Button()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.lblFileUpload = New System.Windows.Forms.Label()
        Me.lblCommandBlock = New System.Windows.Forms.Label()
        Me.btnClose = New System.Windows.Forms.Button()
        Me.pnlTopBorder = New System.Windows.Forms.Panel()
        Me.btnMinimize = New System.Windows.Forms.Button()
        Me.lblTitle = New System.Windows.Forms.Label()
        Me.BackgroundWorker1 = New System.ComponentModel.BackgroundWorker()
        Me.pictureBoxProgress = New System.Windows.Forms.PictureBox()
        CType(Me.PictureBox1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.pnlTopBorder.SuspendLayout()
        CType(Me.pictureBoxProgress, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'PictureBox1
        '
        Me.PictureBox1.Image = CType(resources.GetObject("PictureBox1.Image"), System.Drawing.Image)
        Me.PictureBox1.Location = New System.Drawing.Point(493, 33)
        Me.PictureBox1.Name = "PictureBox1"
        Me.PictureBox1.Size = New System.Drawing.Size(304, 115)
        Me.PictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage
        Me.PictureBox1.TabIndex = 0
        Me.PictureBox1.TabStop = False
        '
        'btnScanPorts
        '
        Me.btnScanPorts.BackColor = System.Drawing.Color.Silver
        Me.btnScanPorts.FlatStyle = System.Windows.Forms.FlatStyle.Flat
        Me.btnScanPorts.Location = New System.Drawing.Point(10, 38)
        Me.btnScanPorts.Name = "btnScanPorts"
        Me.btnScanPorts.Size = New System.Drawing.Size(75, 23)
        Me.btnScanPorts.TabIndex = 10
        Me.btnScanPorts.Text = "Scan Ports"
        Me.btnScanPorts.UseVisualStyleBackColor = False
        '
        'btnWriteCommand
        '
        Me.btnWriteCommand.BackColor = System.Drawing.Color.Silver
        Me.btnWriteCommand.FlatStyle = System.Windows.Forms.FlatStyle.Flat
        Me.btnWriteCommand.Location = New System.Drawing.Point(10, 67)
        Me.btnWriteCommand.Name = "btnWriteCommand"
        Me.btnWriteCommand.Size = New System.Drawing.Size(75, 23)
        Me.btnWriteCommand.TabIndex = 2
        Me.btnWriteCommand.Text = "Write CMD"
        Me.btnWriteCommand.UseVisualStyleBackColor = False
        '
        'btnConnect
        '
        Me.btnConnect.BackColor = System.Drawing.Color.Silver
        Me.btnConnect.FlatAppearance.BorderColor = System.Drawing.Color.Silver
        Me.btnConnect.FlatAppearance.BorderSize = 0
        Me.btnConnect.FlatStyle = System.Windows.Forms.FlatStyle.Flat
        Me.btnConnect.ForeColor = System.Drawing.Color.Black
        Me.btnConnect.Location = New System.Drawing.Point(412, 38)
        Me.btnConnect.Name = "btnConnect"
        Me.btnConnect.Size = New System.Drawing.Size(75, 23)
        Me.btnConnect.TabIndex = 3
        Me.btnConnect.Text = "Connect"
        Me.btnConnect.UseVisualStyleBackColor = False
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.ForeColor = System.Drawing.Color.White
        Me.Label1.Location = New System.Drawing.Point(218, 42)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(61, 13)
        Me.Label1.TabIndex = 4
        Me.Label1.Text = "Baud Rate:"
        '
        'cmbSerialPort
        '
        Me.cmbSerialPort.BackColor = System.Drawing.Color.Black
        Me.cmbSerialPort.FlatStyle = System.Windows.Forms.FlatStyle.Flat
        Me.cmbSerialPort.ForeColor = System.Drawing.Color.White
        Me.cmbSerialPort.FormattingEnabled = True
        Me.cmbSerialPort.Items.AddRange(New Object() {"", "", "Null"})
        Me.cmbSerialPort.Location = New System.Drawing.Point(92, 39)
        Me.cmbSerialPort.Name = "cmbSerialPort"
        Me.cmbSerialPort.Size = New System.Drawing.Size(121, 21)
        Me.cmbSerialPort.TabIndex = 5
        '
        'cmbBaud
        '
        Me.cmbBaud.BackColor = System.Drawing.Color.Black
        Me.cmbBaud.FlatStyle = System.Windows.Forms.FlatStyle.Flat
        Me.cmbBaud.ForeColor = System.Drawing.Color.White
        Me.cmbBaud.FormattingEnabled = True
        Me.cmbBaud.Items.AddRange(New Object() {"9600", "14400", "19200", "28800", "31250", "38400", "57600", "115200"})
        Me.cmbBaud.Location = New System.Drawing.Point(284, 39)
        Me.cmbBaud.Name = "cmbBaud"
        Me.cmbBaud.Size = New System.Drawing.Size(121, 21)
        Me.cmbBaud.TabIndex = 6
        '
        'ListBox1
        '
        Me.ListBox1.BackColor = System.Drawing.Color.Black
        Me.ListBox1.ForeColor = System.Drawing.Color.White
        Me.ListBox1.FormattingEnabled = True
        Me.ListBox1.Location = New System.Drawing.Point(12, 211)
        Me.ListBox1.Name = "ListBox1"
        Me.ListBox1.Size = New System.Drawing.Size(408, 121)
        Me.ListBox1.TabIndex = 11
        '
        'lblDate
        '
        Me.lblDate.ForeColor = System.Drawing.Color.White
        Me.lblDate.Location = New System.Drawing.Point(414, 71)
        Me.lblDate.Name = "lblDate"
        Me.lblDate.Size = New System.Drawing.Size(70, 15)
        Me.lblDate.TabIndex = 12
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.BackColor = System.Drawing.Color.Transparent
        Me.Label2.ForeColor = System.Drawing.Color.White
        Me.Label2.Location = New System.Drawing.Point(369, 73)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(33, 13)
        Me.Label2.TabIndex = 13
        Me.Label2.Text = "Date:"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.ForeColor = System.Drawing.Color.White
        Me.Label3.Location = New System.Drawing.Point(369, 103)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(33, 13)
        Me.Label3.TabIndex = 14
        Me.Label3.Text = "Time:"
        '
        'lblTime
        '
        Me.lblTime.ForeColor = System.Drawing.Color.White
        Me.lblTime.Location = New System.Drawing.Point(414, 103)
        Me.lblTime.Name = "lblTime"
        Me.lblTime.Size = New System.Drawing.Size(70, 15)
        Me.lblTime.TabIndex = 15
        '
        'SerialPort1
        '
        '
        'Timer1
        '
        Me.Timer1.Interval = 1000
        '
        'lblStreamData
        '
        Me.lblStreamData.AutoSize = True
        Me.lblStreamData.ForeColor = System.Drawing.Color.White
        Me.lblStreamData.Location = New System.Drawing.Point(12, 181)
        Me.lblStreamData.Name = "lblStreamData"
        Me.lblStreamData.Size = New System.Drawing.Size(120, 13)
        Me.lblStreamData.TabIndex = 16
        Me.lblStreamData.Text = "Input Stream Data : 000"
        '
        'lblTest
        '
        Me.lblTest.AutoSize = True
        Me.lblTest.ForeColor = System.Drawing.Color.White
        Me.lblTest.Location = New System.Drawing.Point(16, 346)
        Me.lblTest.Name = "lblTest"
        Me.lblTest.Size = New System.Drawing.Size(39, 13)
        Me.lblTest.TabIndex = 17
        Me.lblTest.Text = "Label4"
        '
        'txtWriteCommand
        '
        Me.txtWriteCommand.BackColor = System.Drawing.Color.Black
        Me.txtWriteCommand.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.txtWriteCommand.ForeColor = System.Drawing.Color.White
        Me.txtWriteCommand.Location = New System.Drawing.Point(92, 69)
        Me.txtWriteCommand.Name = "txtWriteCommand"
        Me.txtWriteCommand.Size = New System.Drawing.Size(121, 20)
        Me.txtWriteCommand.TabIndex = 19
        '
        'fileOpen
        '
        Me.fileOpen.FileName = "OpenFileDialog1"
        '
        'btnOpen
        '
        Me.btnOpen.BackColor = System.Drawing.Color.Silver
        Me.btnOpen.FlatStyle = System.Windows.Forms.FlatStyle.Flat
        Me.btnOpen.Location = New System.Drawing.Point(10, 96)
        Me.btnOpen.Name = "btnOpen"
        Me.btnOpen.Size = New System.Drawing.Size(75, 23)
        Me.btnOpen.TabIndex = 20
        Me.btnOpen.Text = "Open File"
        Me.btnOpen.UseVisualStyleBackColor = False
        '
        'btnSendBinaryFile
        '
        Me.btnSendBinaryFile.BackColor = System.Drawing.Color.Silver
        Me.btnSendBinaryFile.FlatStyle = System.Windows.Forms.FlatStyle.Flat
        Me.btnSendBinaryFile.Location = New System.Drawing.Point(10, 125)
        Me.btnSendBinaryFile.Name = "btnSendBinaryFile"
        Me.btnSendBinaryFile.Size = New System.Drawing.Size(75, 23)
        Me.btnSendBinaryFile.TabIndex = 22
        Me.btnSendBinaryFile.Text = "Send Local"
        Me.btnSendBinaryFile.UseVisualStyleBackColor = False
        '
        'GroupBox1
        '
        Me.GroupBox1.ForeColor = System.Drawing.Color.White
        Me.GroupBox1.Location = New System.Drawing.Point(588, 334)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(200, 100)
        Me.GroupBox1.TabIndex = 23
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "GroupBox1"
        '
        'lblFileUpload
        '
        Me.lblFileUpload.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.lblFileUpload.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblFileUpload.Location = New System.Drawing.Point(92, 126)
        Me.lblFileUpload.Name = "lblFileUpload"
        Me.lblFileUpload.Size = New System.Drawing.Size(121, 20)
        Me.lblFileUpload.TabIndex = 24
        Me.lblFileUpload.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblCommandBlock
        '
        Me.lblCommandBlock.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblCommandBlock.Location = New System.Drawing.Point(92, 97)
        Me.lblCommandBlock.Name = "lblCommandBlock"
        Me.lblCommandBlock.Size = New System.Drawing.Size(121, 20)
        Me.lblCommandBlock.TabIndex = 25
        Me.lblCommandBlock.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'btnClose
        '
        Me.btnClose.BackColor = System.Drawing.Color.DimGray
        Me.btnClose.FlatAppearance.BorderColor = System.Drawing.SystemColors.ButtonShadow
        Me.btnClose.FlatStyle = System.Windows.Forms.FlatStyle.Flat
        Me.btnClose.Location = New System.Drawing.Point(768, 3)
        Me.btnClose.Name = "btnClose"
        Me.btnClose.Size = New System.Drawing.Size(29, 24)
        Me.btnClose.TabIndex = 27
        Me.btnClose.Text = "X"
        Me.btnClose.TextAlign = System.Drawing.ContentAlignment.BottomCenter
        Me.btnClose.UseVisualStyleBackColor = False
        '
        'pnlTopBorder
        '
        Me.pnlTopBorder.BackColor = System.Drawing.Color.DimGray
        Me.pnlTopBorder.Controls.Add(Me.btnMinimize)
        Me.pnlTopBorder.Controls.Add(Me.lblTitle)
        Me.pnlTopBorder.Controls.Add(Me.btnClose)
        Me.pnlTopBorder.Location = New System.Drawing.Point(0, 0)
        Me.pnlTopBorder.Name = "pnlTopBorder"
        Me.pnlTopBorder.Size = New System.Drawing.Size(800, 30)
        Me.pnlTopBorder.TabIndex = 28
        '
        'btnMinimize
        '
        Me.btnMinimize.BackColor = System.Drawing.Color.DimGray
        Me.btnMinimize.FlatAppearance.BorderColor = System.Drawing.SystemColors.ButtonShadow
        Me.btnMinimize.FlatStyle = System.Windows.Forms.FlatStyle.Flat
        Me.btnMinimize.Location = New System.Drawing.Point(740, 3)
        Me.btnMinimize.Name = "btnMinimize"
        Me.btnMinimize.Size = New System.Drawing.Size(29, 24)
        Me.btnMinimize.TabIndex = 29
        Me.btnMinimize.Text = "_"
        Me.btnMinimize.UseVisualStyleBackColor = False
        '
        'lblTitle
        '
        Me.lblTitle.AutoSize = True
        Me.lblTitle.Font = New System.Drawing.Font("Microsoft Sans Serif", 11.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblTitle.Location = New System.Drawing.Point(3, 6)
        Me.lblTitle.Name = "lblTitle"
        Me.lblTitle.Size = New System.Drawing.Size(226, 18)
        Me.lblTitle.TabIndex = 28
        Me.lblTitle.Text = "Shooting Target Remote GUI"
        '
        'pictureBoxProgress
        '
        Me.pictureBoxProgress.Location = New System.Drawing.Point(10, 154)
        Me.pictureBoxProgress.Name = "pictureBoxProgress"
        Me.pictureBoxProgress.Size = New System.Drawing.Size(203, 24)
        Me.pictureBoxProgress.TabIndex = 30
        Me.pictureBoxProgress.TabStop = False
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BackColor = System.Drawing.Color.Black
        Me.ClientSize = New System.Drawing.Size(800, 450)
        Me.Controls.Add(Me.pictureBoxProgress)
        Me.Controls.Add(Me.pnlTopBorder)
        Me.Controls.Add(Me.lblCommandBlock)
        Me.Controls.Add(Me.lblFileUpload)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.btnSendBinaryFile)
        Me.Controls.Add(Me.btnOpen)
        Me.Controls.Add(Me.txtWriteCommand)
        Me.Controls.Add(Me.lblTest)
        Me.Controls.Add(Me.lblStreamData)
        Me.Controls.Add(Me.lblTime)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.lblDate)
        Me.Controls.Add(Me.ListBox1)
        Me.Controls.Add(Me.cmbBaud)
        Me.Controls.Add(Me.cmbSerialPort)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.btnConnect)
        Me.Controls.Add(Me.btnWriteCommand)
        Me.Controls.Add(Me.btnScanPorts)
        Me.Controls.Add(Me.PictureBox1)
        Me.ForeColor = System.Drawing.Color.Black
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None
        Me.Name = "Form1"
        Me.ShowIcon = False
        Me.Text = "Shooting Target RF Control Tool"
        CType(Me.PictureBox1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.pnlTopBorder.ResumeLayout(False)
        Me.pnlTopBorder.PerformLayout()
        CType(Me.pictureBoxProgress, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents FolderBrowserDialog1 As FolderBrowserDialog
    Friend WithEvents PictureBox1 As PictureBox
    Friend WithEvents btnScanPorts As Button
    Friend WithEvents btnWriteCommand As Button
    Friend WithEvents btnConnect As Button
    Friend WithEvents Label1 As Label
    Friend WithEvents cmbSerialPort As ComboBox
    Friend WithEvents cmbBaud As ComboBox
    Friend WithEvents ListBox1 As ListBox
    Friend WithEvents lblDate As Label
    Friend WithEvents Label2 As Label
    Friend WithEvents Label3 As Label
    Friend WithEvents lblTime As Label
    Friend WithEvents SerialPort1 As IO.Ports.SerialPort
    Friend WithEvents Timer1 As Timer
    Friend WithEvents lblStreamData As Label
    Friend WithEvents lblTest As Label
    Friend WithEvents txtWriteCommand As TextBox
    Friend WithEvents fileOpen As OpenFileDialog
    Friend WithEvents btnOpen As Button
    Friend WithEvents btnSendBinaryFile As Button
    Friend WithEvents GroupBox1 As GroupBox
    Friend WithEvents lblFileUpload As Label
    Friend WithEvents lblCommandBlock As Label
    Friend WithEvents btnClose As Button
    Friend WithEvents pnlTopBorder As Panel
    Friend WithEvents lblTitle As Label
    Friend WithEvents btnMinimize As Button
    Friend WithEvents BackgroundWorker1 As System.ComponentModel.BackgroundWorker
    Friend WithEvents pictureBoxProgress As PictureBox
End Class
