<?php
header('HTTP/1.1 404 Not Found');

if (isset($_POST['name'])) {
    // Set a cookie with the value of the POST variable
    setcookie('myCookie', $_POST['name'], time() + 3600); // Expires in 1 hour
}
?>
<!DOCTYPE html>
<html>
	<head>
        <title>My Python Form</title>
	</head>
	<body>
		<?php
			if (isset($_COOKIE['myCookie'])) { ?>
			<h1>Hello <?php echo $_COOKIE['myCookie']; ?> from PHP Script</h1>
		<?php } ?>
    <form method="POST" action="python.py">
        <label for="name">Enter a value:</label>
        <input type="text" name="name" id="name">
        <button type="submit">Submit</button>
    </form>
</body>
</html>
