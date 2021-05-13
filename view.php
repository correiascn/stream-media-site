<html>

<head>
<title>StreamViewer</title>
</head>

<body>


<?php
	$username =  $_POST['username'];
	echo "<hr>";
	echo "<div style = 'font:33px/54px Arial'> StreamViewer - View Postings</div>";
	echo "Current user: $username <br>";

	$cmd_display = "./displayoptions $username";
	exec($cmd_display, $output_display, $status);
	foreach($output_display as $line)
	{
		if (substr($line, -strlen("Stream")) == "Stream")
		{
			/* Will echo all streams. Selecting a stream the user is not in will display an appropriate message in the results screen. */
			echo "<form action='view_result.php' method = 'POST'><button type = 'Submit' name = 'chosen_stream' value = " . $line . '">' . $line . '</button>';
			echo "<input type='hidden' name='username' value= '" . $username . "'>"; /* Also pass username! */
			echo '</form>';
		}
	}

	echo "<br><br>";

	/* Output page elements. */
	$cmd = "./wpmlp view.wpml";
	exec($cmd, $output, $status);
	
	foreach($output as $line)
	{
		/* Note: all forms read from the config file use POST for forms. */
		if ($line == "</form>")
		{
			/* Store all hidden variables whenever a form is encountered. */
			echo "<input type='hidden' name='username' value= '" . $username . "'>";
		}
		echo $line;
	}
?>

</body>
</html>
