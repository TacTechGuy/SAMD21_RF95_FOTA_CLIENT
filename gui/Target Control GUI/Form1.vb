
Imports System
Imports System.IO
Imports System.IO.Ports
Imports System.Text.RegularExpressions



Public Class Form1
    Dim fileFlag As Boolean = False
    Dim connectFlag As Boolean = False
    Dim readBytes() As Byte                         'used for the binary file array
    Dim fileSize As Integer
    Dim fileCommand(34) As Byte
    Dim fileTransferFlag As Boolean = False
    Dim timerCounter As Integer = 6000
    Dim pbHideFlag As Boolean = False

    ' Picture box converted into a progress bar
    Dim progressBarUnit As Double
    Dim progressBarWidth, progressBarHeight, progressBarComplete As Integer
    Dim bmp As Bitmap
    Dim g As Graphics


    ' This code is used to move the form since we do not have a border
    Private Const HTCLIENT As Integer = &H1
    Private Const HTCAPTION As Integer = &H2
    Private Const WM_NCHITTEST As Integer = &H84

    Protected Overrides Sub WndProc(ByRef m As System.Windows.Forms.Message)
        MyBase.WndProc(m)

        If m.Msg = WM_NCHITTEST AndAlso m.Result = HTCLIENT Then
            m.Result = HTCAPTION
        End If
    End Sub


    Private newpoint As System.Drawing.Point
    Private xpos1 As Integer
    Private ypos1 As Integer

    Private Sub pnlTopBorder_MouseDown(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles pnlTopBorder.MouseDown
        xpos1 = Control.MousePosition.X - Me.Location.X
        ypos1 = Control.MousePosition.Y - Me.Location.Y
    End Sub

    Private Sub pnlTopBorder_MouseMove(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles pnlTopBorder.MouseMove
        If e.Button = System.Windows.Forms.MouseButtons.Left Then
            newpoint = Control.MousePosition
            newpoint.X -= (xpos1)
            newpoint.Y -= (ypos1)
            Me.Location = newpoint
        End If
    End Sub

    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        Me.CenterToParent()
        ' Get the dimensions
        initalizePictureBox_PB()


        'Used to handle the cross form issue when debugging
        System.Windows.Forms.Control.CheckForIllegalCrossThreadCalls = False

        'function for getting the time
        dateTime()


        'disable buttons
        btnConnect.Enabled = False
        btnConnect.Visible = False

        btnSendBinaryFile.Enabled = False
        btnWriteCommand.Enabled = False
        btnOpen.Enabled = False

        ' *** This was used at the start to put a message in the serial port combo box ***
        'cmbSerialPort.SelectedText = "hey there"
        btnConnect.ForeColor = Color.Black


        lblFileUpload.Visible = False
        btnSendBinaryFile.Visible = False


        PictureBox1.Focus()
    End Sub

    Private Sub btnConnect_Click(sender As Object, e As EventArgs) Handles btnConnect.Click
        connectButtonChange()


        If (btnConnect.Text = "Connect" And cmbBaud.Text <> Nothing And cmbSerialPort.Text <> Nothing) Then
            btnConnect.Text = "Disconnect"
            connectFlag = False
            btnScanPorts.Enabled = False

            SerialPort1.BaudRate = cmbBaud.SelectedItem
            SerialPort1.PortName = cmbSerialPort.SelectedItem

            SerialPort1.Open()

            'write the command to the board
            SerialPort1.WriteLine("get state")
            Timer1.Start()


        ElseIf (btnConnect.Text = "Disconnect") Then
            btnConnect.Text = "Connect"
            btnScanPorts.Enabled = True
            connectFlag = True

            btnSendBinaryFile.Visible = False
            lblFileUpload.Visible = False
            'pictureBoxProgress.Visible = False

            'lets go ahead and clear out the lblStreamData
            lblStreamData.Text = "Input Stream Data : "

            Timer1.Stop()
            SerialPort1.Close()
        End If

        'can use this also to enable/ disable the writeCommand / openfile / send binary buttons
        If (connectFlag = False) Then
            btnConnect.BackColor = Color.Red
            btnConnect.ForeColor = Color.White
            'enable the buttons
            btnWriteCommand.Enabled = True
            btnOpen.Enabled = True


            'enable the combo boxes
            'cmbBaud.Enabled = True
            ' cmbSerialPort.Enabled = True


        ElseIf (connectFlag = True And cmbSerialPort.Text <> Nothing And cmbBaud.Text <> Nothing) Then
            btnConnect.BackColor = Color.Green
            btnConnect.ForeColor = Color.Black
            'Make sure to disable the fileFlag
            fileFlag = False

            'disable the buttons
            btnWriteCommand.Enabled = False
            btnOpen.Enabled = False
            btnSendBinaryFile.Enabled = False
            ' Change the send binary file button wording to the intial value
            btnSendBinaryFile.Text = "Send Local"
            'color the labels
            lblCommandBlock.BackColor = Color.Black   'changed from SystemColors.Control
            lblCommandBlock.Text = " "
            lblFileUpload.BackColor = SystemColors.Control
            lblFileUpload.Text = " "

            'clear out the listbox
            ListBox1.Items.Clear()

            'diable the combo boxes
            'cmbBaud.Enabled = False
            'cmbSerialPort.Enabled = False
        End If



        PictureBox1.Focus()
    End Sub





    Private Sub cmbBaud_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmbBaud.SelectedIndexChanged
        PictureBox1.Focus()

        'meaning that both of the combo boxes aren't empty
        connectButtonChange()
    End Sub

    Private Sub cmbSerialPort_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmbSerialPort.SelectedIndexChanged
        PictureBox1.Focus()

        connectButtonChange()
    End Sub

    Private Sub btnScanPorts_Click(sender As Object, e As EventArgs) Handles btnScanPorts.Click
        PictureBox1.Focus()
        ' MsgBox($"Connect flag is set to: {connectFlag} " & vbNewLine & $"Connect button status: {btnConnect.Enabled}")


        cmbSerialPort.Items.Clear()

        Dim myPort As Array
        Dim i As Integer
        myPort = IO.Ports.SerialPort.GetPortNames()
        cmbSerialPort.Items.AddRange(myPort)

        ' ListBox1.Items.AddRange(myPort)   *** Was only used to list the COM ports in the listbox ***

        i = cmbSerialPort.Items.Count
        i = i - i

        Try
            cmbSerialPort.SelectedIndex = i
        Catch ex As Exception
            Dim result As DialogResult
            result = MessageBox.Show("com port not detected", "Warning !!!", MessageBoxButtons.OK)
            cmbSerialPort.Text = " "
            cmbSerialPort.Items.Clear()
            Call Form1_Load(Me, e)
        End Try
        connectButtonChange()
        'btnConnect.Enabled = True

        'cmbSerialPort.DroppedDown = True
    End Sub

    Private Sub btnWriteCommand_Click(sender As Object, e As EventArgs) Handles btnWriteCommand.Click


        Dim byteVal As Byte

        Try
            If Byte.TryParse(txtWriteCommand.Text, byteVal) Then
                Dim SerialBuffer(1) As Byte
                SerialBuffer(0) = byteVal

                SerialPort1.Write(SerialBuffer, 0, 1)
            Else
                Dim inputCommand As String = txtWriteCommand.Text
                Dim fixedInput As String
                fixedInput = Trim(inputCommand).ToLower()

                If fixedInput = "file transfer" And fileFlag = False Then
                    fixedInput = Nothing
                    ' display a warning message in the listbox
                    ListBox1.Items.Add("*** File needs to be opened first ***" & vbTab & vbTab & Now.ToString("T"))
                Else
                    SerialPort1.WriteLine(fixedInput)
                End If


            End If

        Catch ex As Exception

        End Try



        'Dim value As Byte = Convert.ToByte(txtWriteCommand.Text)




        '**** THIS WAS THE OLD WAY OF SENDING BYTE DATA TO THE MICROCONTROLLER ****
        'Dim inputCommandResult As String = Regex.Replace(inputCommand, "0x", "")
        '    Dim SerialBuffer(5) As Byte
        '    SerialBuffer(0) = Convert.ToByte(inputCommandResult)  'txtWriteCommand.text
        '    SerialPort1.Write(SerialBuffer, 0, 1)










        'MsgBox(inputCommandResult)



        'SerialPort1.Write(txtWriteCommand.Text)
        'SerialPort1.Write(txtWriteCommand.Text)

        'clear the text box
        txtWriteCommand.Text = " "

        'return focus to the picturebox
        PictureBox1.Focus()

    End Sub

    'this is for checking the combo boxes and then adjusting the connect button accordingly
    Private Sub connectButtonChange()
        If (cmbSerialPort.Text <> Nothing And cmbBaud.Text <> Nothing) Then
            btnConnect.Enabled = True
            btnConnect.Visible = True
            btnConnect.BackColor = Color.Green
            btnConnect.ForeColor = Color.Black
        Else
            'meaning we have disconencted 
            btnConnect.BackColor = SystemColors.Control
            btnConnect.ForeColor = SystemColors.ControlText
            btnConnect.Enabled = False
        End If

    End Sub

    Private Sub initalizePictureBox_PB()
        progressBarWidth = pictureBoxProgress.Width
        progressBarHeight = pictureBoxProgress.Height
        progressBarUnit = progressBarWidth / 100
        progressBarComplete = 0

        bmp = New Bitmap(progressBarWidth, progressBarHeight)
    End Sub


    Sub dateTime()
        'get the current date from the system
        Dim dtmSytemDate = Now

        'display the data in the lblDate text property
        lblDate.Text = dtmSytemDate.ToString("d")
        lblTime.Text = dtmSytemDate.ToString("T")
    End Sub

    Private Sub Timer1_Tick(sender As Object, e As EventArgs) Handles Timer1.Tick




        If pbHideFlag = True Then
            If timerCounter = 0 Then

                pictureBoxProgress.Visible = False

                'Timer1.Enabled = False
                pbHideFlag = False

                lblFileUpload.Text = "PUSH FILE REMOTE"      'Change the status to show that we are ready to send remotely
                btnSendBinaryFile.Text = "Send RMT"          'Change the button text

                'timerCounter = 6000            'reset back to the standard 6's delay

                'lets renable the send to get ready for the remote sending of the file
                btnSendBinaryFile.Enabled = True
            Else
                timerCounter -= 1000

            End If
        ElseIf fileTransferFlag = True Then
            If timerCounter = 0 Then
                pictureBoxProgress.Visible = False
                'Timer1.Enabled = False

                'timerCounter = 6000
                btnOpen.Enabled = True

                btnSendBinaryFile.Visible = False
                lblFileUpload.Visible = False

                fileTransferFlag = False
                fileFlag = False                         'main flag

            Else
                timerCounter -= 1000
            End If
        End If





        'Try
        'Dim inputStream As String = SerialPort1.ReadExisting

        'lblStreamData.Text = "Input Stream Data : " & inputStream.ToString

        'Catch ex As Exception

        'End Try



    End Sub



    Private Sub SerialPort1_DataReceived(sender As Object, e As SerialDataReceivedEventArgs) Handles SerialPort1.DataReceived
        Try

            Dim fileTransferComplete As String = "file transfer complete" & vbCr
            Dim pattern As String = "\bpb\d+\b"  'used to extract the number of bytes sent so we can update the progress bar


            Dim printData As String = Nothing
            ' [x] Dim inputStream As Integer = SerialPort1.ReadExisting
            '**** Block out the message box ****
            'MsgBox(inputStream)

            '**** Block the text box and lets just use the list box **** 
            ' [x] lblStreamData.Text = "Input Stream Data : " & inputStream

            '**** Used to convert ****
            'Dim inputNumber As Integer = Convert.ToInt32(SerialPort1.ReadExisting)
            Dim incomingData As String = Convert.ToString(SerialPort1.ReadLine)


            '** Used to update the progress bar
            If Regex.IsMatch(incomingData, pattern) Then
                'strip the pb off of the string
                Dim data As Integer = Convert.ToInt32(incomingData.Substring(2))

                ' Convert the incoming data into  a percentage of the filesize
                progressBarComplete = (data / fileSize) * 100
                If progressBarComplete > 97 And progressBarComplete < 100 Then
                    progressBarComplete = 100
                ElseIf progressBarComplete > 100 Then
                    progressBarComplete = 100
                End If

                ' Draw progress bar
                g = Graphics.FromImage(bmp)
                g.Clear(Color.LightGray)
                g.FillRectangle(Brushes.Green, New Rectangle(0, 0, CInt(progressBarComplete * progressBarUnit), progressBarHeight))

                ' Place text data
                g.DrawString(progressBarComplete & "%", New Font("Segoe UI", progressBarHeight / 2), Brushes.Black, New PointF(progressBarWidth / 2 - progressBarHeight, progressBarHeight / 12))

                pictureBoxProgress.Image = bmp

                incomingData = Nothing

            End If

            'If incomingData = fileTransferComplete Then
            '    'set the timer for the button


            '    fileTransferFlag = True

            '    lblFileUpload.BackColor = Color.Green
            '    lblFileUpload.Text = "Complete"

            '    printData = incomingData
            'End If

            'If incomingData = "main" & vbCr Then
            '    lblCommandBlock.BackColor = Color.Green
            '    lblCommandBlock.Text = "MAIN"
            'ElseIf incomingData = "command" & vbCr Then
            '    lblCommandBlock.BackColor = Color.Aqua
            '    lblCommandBlock.Text = "CMD BLK"
            'End If



            Select Case incomingData
                Case "file transfer complete" & vbCr
                    'fileTransferFlag = True
                    lblFileUpload.BackColor = Color.Green
                    'lblFileUpload.Text = "Complete"
                    printData = incomingData
                    '*** Re-enable the file open button ***
                    btnOpen.Enabled = True

                Case "file transfer block" & vbCr
                    lblCommandBlock.BackColor = Color.Purple
                    lblCommandBlock.Text = "LOC-TRANS. BLK"
                    '*** Need to disable the open file button ***

                    If fileFlag Then
                        btnOpen.Enabled = False
                    End If



                    'once we have a file then we can enable the send file button
                    btnSendBinaryFile.Enabled = True
                    'color the textbox 
                    lblFileUpload.BackColor = Color.Green
                    lblFileUpload.Text = "PUSH FILE LOCAL"

                Case "remote transfer block" & vbCr
                    lblCommandBlock.BackColor = Color.Purple
                    lblCommandBlock.Text = "RMT-TRANS. BLK"

                Case "file command block" & vbCr
                    lblCommandBlock.BackColor = Color.Aqua
                    lblCommandBlock.Text = "FILE CMD BLK"
                ' Once we have finished sending the file we need to return to main and then check the integrity of the file
                Case "remote file transfer complete" & vbCr

                    fileTransferFlag = True
                    timerCounter = 3000
                    Me.Timer1.Enabled = True

                    lblCommandBlock.BackColor = Color.Green
                    lblCommandBlock.Text = "MAIN CMD BLK"

                    lblFileUpload.BackColor = Color.Green
                    lblFileUpload.Text = "Complete"

                Case "main" & vbCr
                    lblCommandBlock.BackColor = Color.Green
                    lblCommandBlock.Text = "MAIN CMD BLK"
                    ' This means the we have hit the timeout for the file transfer
                    ' so we are back on the main control block
                    If lblFileUpload.Text = "PUSH FILE LOCAL" And lblCommandBlock.Text = "MAIN CMD BLK" Then
                        'disable the send button and prompt for the correct command
                        btnSendBinaryFile.Enabled = False

                        lblFileUpload.Text = "WAITING FOR CMD"
                        lblFileUpload.BackColor = Color.Orange
                    End If

                Case "match" & vbCr
                    lblFileUpload.BackColor = Color.Green
                    lblFileUpload.Text = "Complete"


                    pbHideFlag = True
                    timerCounter = 3000
                    Me.Timer1.Enabled = True 'start the timer

                    btnOpen.Enabled = False  'disable the open file button'
                Case Else
                    printData = incomingData
            End Select

            If printData <> Nothing Then
                ListBox1.Items.Add(printData)
            End If



        Catch ex As Exception

        End Try

    End Sub

    Private Sub btnOpen_Click(sender As Object, e As EventArgs) Handles btnOpen.Click
        'opens the file explorer navigation box
        'fileOpen.ShowDialog()

        lblFileUpload.BackColor = SystemColors.Control
        lblFileUpload.Text = " "

        Try
            'this will return the file extension and we'll make sure that its a binary file
            If fileOpen.ShowDialog() = DialogResult.OK Then
                Dim fileType As String = Path.GetExtension(fileOpen.FileName)
                'ListBox1.Items.Add(fileType)

                If fileType.Equals(".bin") Then
                    fileFlag = True
                Else
                    fileFlag = False
                    MsgBox("must select a .bin file")
                End If


            Else
                'If we open but then for some reason we don't select a file
                'ListBox1.Items.Add("Need to select a file")
                lblFileUpload.BackColor = Color.Orange
                lblFileUpload.Text = "FILE NOT SELECTED"
                fileOpen.Dispose()
            End If

            Try
                If fileFlag Then
                    'create the filestream object 'inputStream'
                    Dim inputStream As New FileStream(fileOpen.FileName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite)
                    Dim size As Integer = inputStream.Length

                    'save file size to global variable
                    fileSize = size

                    ListBox1.Items.Add("File size:" & size)

                    'use the binary reader to read each byte
                    Dim bReader As New BinaryReader(inputStream)
                    inputStream.Position = 0

                    'read all the values
                    readBytes = bReader.ReadBytes(size)

                    'put the values in listBox1
                    'For Each numByte In readBytes
                    'ListBox1.Items.Add(numByte)
                    'Next
                    'add count to the listbox to see if it is correct

                    '*** This generates a Hash from the file ***
                    generateFileHash(readBytes)


                    'close the reader and stream

                    bReader.Close()
                    inputStream.Dispose()

                    ' **** JUST FOR TESTING ****
                    'just a test to see if it prints the correct index value
                    'ListBox1.Items.Add("This is from index 1: " & readBytes(1))

                    'once we have a file then we can enable the send file button
                    'btnSendBinaryFile.Enabled = True

                    'reset the filename
                    fileOpen.FileName = Nothing

                    ''color the textbox 
                    'lblFileUpload.BackColor = Color.Red
                    'lblFileUpload.Text = "Ready"

                    'Lets then go ahead and make the lbl box and the button visible
                    lblFileUpload.Visible = True
                    btnSendBinaryFile.Visible = True

                    If btnSendBinaryFile.Text <> "Send Local" Then
                        btnSendBinaryFile.Text = "Send Local"
                    End If


                    lblFileUpload.Text = "WAITING FOR CMD"
                    lblFileUpload.BackColor = Color.Orange

                End If
            Catch ex As Exception

            End Try

            'return focus to the pictureBox1
            PictureBox1.Focus()

        Catch ex As Exception
            'MsgBox("we have errors opening the file")   
        End Try


    End Sub

    'This is used to send the data from the computer to the board over Serial
    Public Sub sendBinaryFile(ByRef byteArray() As Byte)

    End Sub

    Private Sub btnSendBinaryFile_Click(sender As Object, e As EventArgs) Handles btnSendBinaryFile.Click

        If lblFileUpload.Text = "PUSH FILE REMOTE" Then
            'disable the send button
            btnSendBinaryFile.Enabled = False

            'make the [Picture Box] progress bar visible 
            pictureBoxProgress.Visible = True
            progressBarComplete = 0

            ' Update the pictureBoxProgress bar
            g = Graphics.FromImage(bmp)
            g.Clear(Color.LightGray)
            g.FillRectangle(Brushes.Green, New Rectangle(0, 0, CInt(progressBarComplete * progressBarUnit), progressBarHeight))

            ' Place text data
            'g.DrawString(progressBarComplete & "%", New Font("Segoe UI", progressBarHeight / 2), Brushes.Black, New PointF(progressBarWidth / 2 - progressBarHeight, progressBarHeight / 12))

            pictureBoxProgress.Image = bmp


            ' Change the colors
            lblFileUpload.BackColor = Color.PaleGoldenrod
            lblFileUpload.Text = "SENDING REMOTE"

            'lets send the command that is used to push the binary file to the remote device
            SerialPort1.WriteLine("upload remote")

        ElseIf lblFileUpload.Text = "PUSH FILE LOCAL" Then
            pictureBoxProgress.Visible = True

            'disable the send button
            btnSendBinaryFile.Enabled = False

            'before we send the file we need to make sure that we first send a command and then receive a response
            'and we also need to send the size of the firmware so we know what to expect
            lblFileUpload.BackColor = Color.PaleGoldenrod
            lblFileUpload.Text = "SENDING LOCAL"


            'add them to the listbox to see if they are correct
            ' ListBox1.Items.Add(fileCommand(1))
            ' ListBox1.Items.Add(fileCommand(2))

            SerialPort1.Write(fileCommand, 0, fileCommand.Length)  'write the 22 on the wire to the receiver and them make sure we get it back

            '***ListBox1.Items.Add(readBytes(1))

            'should send the readBytes[] that is the Binary file we opened and send over serial to the Sam board
            SerialPort1.Write(readBytes, 0, readBytes.Length)

            'For dataByte As Integer = 0 To readBytes.Length - 1
            '    SerialPort1.Write(readBytes, dataByte, 1)
            'Next

            progressBarComplete = 0

            ' Update the pictureBoxProgress bar
            g = Graphics.FromImage(bmp)
            g.Clear(Color.LightGray)
            g.FillRectangle(Brushes.Green, New Rectangle(0, 0, CInt(progressBarComplete * progressBarUnit), progressBarHeight))

            ' Place text data
            'g.DrawString(progressBarComplete & "%", New Font("Segoe UI", progressBarHeight / 2), Brushes.Black, New PointF(progressBarWidth / 2 - progressBarHeight, progressBarHeight / 12))

            pictureBoxProgress.Image = bmp


            'disable the send file button and change the textBox color back to systemSettings  **Make sure the button stays on for when we need to send it remotely**
            'btnSendBinaryFile.Enabled = False 
        End If





        'return focus to the pictureBox1
        PictureBox1.Focus()

    End Sub

    Sub generateFileHash(ByRef inputArray() As Byte)
        'delare all of the same variables that are in the Arduino program so things can match up
        Dim generatedHash As String = " "
        Dim hashSize As Byte = 0
        Dim count As UInt32 = 0
        Dim bitShiftCount As Byte = 0
        Dim hashBank As UInt32 = 0
        Dim hashBankCount As UInt32 = 0
        Dim stream As UInt32 = 0

        'Array to hold all of the hash values
        Dim hashArray() As UInt32 = {0, &HB9D1FBFDUI, &HFF206483UI, &HC829A0B9UI, &H832459ECUI, &HEFE83F42UI, &HEFB7A415UI, &HDF4BD379UI, &HE21B5202UI}

        'From Arduino code                B9D1FBFD,     FF206483,     C829A0B9,     832459EC,     EFE83F42,     EFB7A415,     DF4BD379,     E21B5202 



        'now I can basically lay out the Hash function like I have in my Arduino Code so the hashes match
        For Each arrayValue In inputArray
            'for loop, make sure that we iterate and increase by one each time, output: 0,1,2,3 (4)
            '***Blocked out because it was giving issues
            'For i As Integer = 0 To 3 Step 1
            stream = arrayValue
            'switch case for the hashbank variable
            Select Case bitShiftCount
                Case 0
                    hashBank = stream << 24
                Case 1
                    hashBank = (hashBank Or (stream << 16))
                Case 2
                    hashBank = (hashBank Or (stream << 8))
                Case 3
                    hashBank = (hashBank Or stream)
            End Select

            'increase the counters
            count += 1
            bitShiftCount += 1

            'check the count
            If count Mod 4 = 0 Then
                'increase the hashbank counter
                hashBankCount += 1

                bitShiftCount = 0

                'store the hashbank within the hashArray after manipulating it
                Select Case hashBankCount
                    Case 1
                        If hashBank <> 0 Then
                            hashArray(1) = (hashBank Xor hashArray(1))
                        Else
                            hashArray(1) = &HC5A14356UI
                            'C5A14356
                        End If
                        'IIf(hashBank <> 0, hashArray(1) = (hashBank Xor hashArray(1)), hashArray(1) = &HC5A14356UI)
                        'reset hashbank
                        hashBank = 0
                    Case 2
                        If hashBank <> 0 Then
                            hashArray(2) = (hashBank Xor (Not hashArray(2)))
                        Else
                            hashArray(2) = &H84014006UI
                            '84014006
                        End If
                        'hashArray(2) = IIf(hashBank <> 0, hashBank = (hashBank Xor (Not hashArray(2))), hashBank = &H84014006UI)
                        'reset hashbank
                        hashBank = 0
                    Case 3
                        If hashBank <> 0 Then
                            hashArray(3) = (hashBank Xor hashArray(3))
                        Else
                            hashArray(3) = &H634DE5A3UI
                            '634DE5A3
                        End If
                        'hashArray(3) = IIf(hashBank <> 0, hashBank = (hashBank Xor hashArray(3)), hashBank = &H634DE5A3UI)
                        'reset hashbank
                        hashBank = 0
                    Case 4
                        If hashBank <> 0 Then
                            hashArray(4) = (hashBank Or (Not hashArray(4)))
                        Else
                            hashArray(4) = &H92F80685UI
                            '92F80685
                        End If
                        'hashArray(4) = IIf(hashBank <> 0, hashBank = (hashBank Or (Not hashArray(4))), hashBank = &H92F80685UI)
                        'reset hashbank
                        hashBank = 0
                    Case 5
                        If hashBank <> 0 Then
                            hashArray(5) = (hashBank Xor (Not hashArray(5)))
                        Else
                            hashArray(5) = &H1FEE62ADUI
                            '1FEE62AD
                        End If
                        'hashArray(5) = IIf(hashBank <> 0, hashBank = (hashBank Xor (Not hashArray(5))), hashBank = &H1FEE62ADUI)
                        'reset hashbank
                        hashBank = 0
                    Case 6
                        If hashBank <> 0 Then
                            hashArray(6) = (hashBank Or (Not hashArray(6)))
                        Else
                            hashArray(6) = &HDA4AC801UI
                            'DA4AC801
                        End If
                        'hashArray(6) = IIf(hashBank <> 0, hashBank = (hashBank Or (Not hashArray(6))), hashBank = &HDA4AC801UI)
                        'reset hashbank
                        hashBank = 0
                    Case 7
                        If hashBank <> 0 Then
                            hashArray(7) = (hashBank Xor hashArray(7))
                        Else
                            hashArray(7) = &H6D800AE8UI
                            '6D800AE8
                        End If
                        'hashArray(7) = IIf(hashBank <> 0, hashBank = (hashBank Xor hashArray(7)), hashBank = &H6D800AE8)
                        'reset hashbank
                        hashBank = 0
                    Case 8
                        If hashBank <> 0 Then
                            hashArray(8) = (hashBank Or (Not hashArray(8)))
                        Else
                            hashArray(8) = &HD46DC441UI
                            'D46DC441
                        End If
                        'hashArray(8) = IIf(hashBank <> 0, hashBank = (hashBank Or (Not hashArray(8))), hashBank = &HD46DC441)
                        'reset hashbank
                        hashBank = 0
                End Select

                'Used to reset the hashBankCount back to zero
                If hashBankCount = 8 Then
                    hashBankCount = 0
                End If

            End If

            ' Next

        Next


        'now we should have our array full of values lets print them to make sure we get the same Hash
        For i As Integer = 1 To 8 Step 1
            generatedHash += hashArray(i).ToString("X8")
            'ListBox1.Items.Add(hashArray(i).ToString("X8"))
            hashSize += Len(hashArray(i))
        Next

        '*** Array was used to test ****
        'Dim arrayCopy(31) As Byte
        Dim start As Byte = 3       'because index 0 = 22, 1 & 2 are for file size

        'used to copy the hashArray() to the fileCommand() 
        For i As Integer = 1 To 8 Step 1
            For j As Integer = 1 To 4 Step 1
                Select Case j
                    Case 1
                        fileCommand(start) = Convert.ToByte(hashArray(i) >> 24 And &HFF)
                        start += 1
                    Case 2
                        fileCommand(start) = Convert.ToByte(hashArray(i) >> 16 And &HFF)
                        start += 1
                    Case 3
                        fileCommand(start) = Convert.ToByte(hashArray(i) >> 8 And &HFF)
                        start += 1
                    Case 4
                        fileCommand(start) = Convert.ToByte(hashArray(i) And &HFF)
                        start += 1
                End Select
            Next
        Next

        fileCommand(0) = 22  'nothing special about 22 was oringinally going to use it as a command on the receive side
        fileCommand(1) = fileSize >> 8 And &HFF
        fileCommand(2) = fileSize And &HFF

        '**** FOR TESTING PURPOSES ****
        'For Each item In fileCommand
        '    ListBox1.Items.Add(item.ToString("X8"))
        'Next

        'Dim bits As Integer = IntPtr.Size * 8
        'ListBox1.Items.Add("operating sytem: " & bits)

        'because after I have the array the way I need it I'll then convert to String
        'generatedHash = hashArray(1).ToString()
        'ListBox1.Items.Add(generatedHash)

        '**** FOR TESTING PURPOSES ****
        'ListBox1.Items.Add(hashSize)

        '**** UNCOMMENT TO SEE HASH ****
        'lblTest.Text = generatedHash

        'Return generatedHash   **NO LONGER A FUNCTION 
    End Sub


    ' Handles the form minimizing and closing because I added a panel to the form 
    Private Sub btnClose_Click(sender As Object, e As EventArgs) Handles btnClose.Click
        Me.Close()
    End Sub

    Private Sub btnMinimize_Click(sender As Object, e As EventArgs) Handles btnMinimize.Click
        Me.WindowState = FormWindowState.Minimized
    End Sub



    ' Handles the mouseHover actions to change the button focus color
    Private Sub btnScanPorts_MouseHover(sender As Object, e As EventArgs) Handles btnScanPorts.MouseHover
        btnScanPorts.FlatAppearance.MouseOverBackColor = Color.LightGreen
    End Sub

    Private Sub btnWriteCommand_MouseHover(sender As Object, e As EventArgs) Handles btnWriteCommand.MouseHover
        btnWriteCommand.FlatAppearance.MouseOverBackColor = Color.LightGreen
    End Sub

    Private Sub btnOpen_MouseHover(sender As Object, e As EventArgs) Handles btnOpen.MouseHover
        btnOpen.FlatAppearance.MouseOverBackColor = Color.LightGreen
    End Sub

    Private Sub btnSendBinaryFile_MouseHover(sender As Object, e As EventArgs) Handles btnSendBinaryFile.MouseHover
        btnSendBinaryFile.FlatAppearance.MouseOverBackColor = Color.LightGreen
    End Sub
End Class


