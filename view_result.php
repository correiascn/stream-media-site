<html>

<head>
<title>StreamViewer</title>
</head>

<body>


<?php
	$chosen_stream = $_POST['chosen_stream'];
	$chosen_stream = rtrim($chosen_stream, '"');
	$index = $_POST['index'];
	$index_change = $_POST['index_change'];
	$sort_type = $_POST['sort_type'];
	$username =  $_POST['username'];

	if ($sort_type == "")
	{
		$sort_type = "f";
	}
	if ($index == "")
	{
		$index = "0";
	}
	if ($index_change == "")
	{
		$index_change = "0";
	}

	if ($index_change == "+")
	{
		$index = $index + 1;
	}
	if ($index_change == "-")
	{
		if ($index > 0)
		{
			$index = $index - 1;
		}
	}

	echo "<hr>";
	echo "<div style = 'font:33px/54px Arial'> StreamViewer - View Postings</div>";
	echo "Current user: $username <br>";
	echo "Chosen stream: $chosen_stream <br><br>";

	/* Output view.py stuff. */
	$progcmd = "/usr/bin/python3 view.py $chosen_stream $index_change $index $sort_type $username";
	/* Debug line, remove upon submission. */
	//echo $progcmd; echo "<br>";
	$output = array();
	exec($progcmd, $output, $status);	
	foreach($output as $line)
	{
		echo $line."<br>";
	}

	echo "<br>";

	/* Output page elements. */
	$cmd = "./wpmlp view_result.wpml";
	$output2 = array();
	exec($cmd, $output2, $status);
	
	foreach($output2 as $line)
	{
		/* Note: all forms read from the config file use POST for forms. */
		if ($line == "</form>")
		{
			/* Store all hidden variables whenever a form is encountered. */
			echo "<input type='hidden' name='username' value= '" . $username . "'>";
			echo "<input type='hidden' name='chosen_stream' value= '" . $chosen_stream . "'>";
		}
		elseif ($line == "\t<button type = 'Submit' name = \"sort_time\" value = \"Sort by Time\">Sort by Time</button>")
		{
			echo "<input type='hidden' name='sort_type' value= 'f'>";
			echo "<input type='hidden' name='index' value= '0'>";
			echo "<input type='hidden' name='index_change' value= '0'>";
		}
		elseif ($line == "\t<button type = 'Submit' name = \"sort_author\" value = \"Sort by Author\">Sort by Author</button>")
		{
			echo "<input type='hidden' name='sort_type' value= 't'>";
			echo "<input type='hidden' name='index' value= '0'>";
			echo "<input type='hidden' name='index_change' value= '0'>";
		}
		elseif ($line == "\t<button type = 'Submit' name = \"check_new\" value = \"Check for New Posts\">Check for New Posts</button>")
		{
			echo "<input type='hidden' name='sort_type' value= '" . $sort_type . "'>";
			echo "<input type='hidden' name='index' value= '" . $index . "'>";
			echo "<input type='hidden' name='index_change' value= '0'>";
		}
		elseif ($line == "\t<button type = 'Submit' name = \"mark_read\" value = \"Mark All Read\">Mark All Read</button>")
		{
			echo "<input type='hidden' name='sort_type' value= '" . $sort_type . "'>";
			echo "<input type='hidden' name='index' value= '" . $index . "'>";
			echo "<input type='hidden' name='index_change' value= 'm'>";
		}
		elseif ($line == "\t<button type = 'Submit' name = \"page_up\" value = \"Previous Post\">Previous Post</button>")
		{
			echo "<input type='hidden' name='sort_type' value= '" . $sort_type . "'>";
			echo "<input type='hidden' name='index' value= '" . $index . "'>";
			echo "<input type='hidden' name='index_change' value= '-'>";
		}
		elseif ($line == "\t<button type = 'Submit' name = \"page_down\" value = \"Next Post\">Next Post</button>")
		{
			echo "<input type='hidden' name='sort_type' value= '" . $sort_type . "'>";
			echo "<input type='hidden' name='index' value= '" . $index . "'>";
			echo "<input type='hidden' name='index_change' value= '+'>";
		}

			echo $line;
	}
?>

</body>
</html>
