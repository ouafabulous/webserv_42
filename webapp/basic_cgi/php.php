<?php
header('HTTP/1.1 404 Not Found');

if (isset($_POST['name']) && $_POST['name'] !== '') {
    // Set a cookie with the value of the POST variable
    setcookie('myCookie', $_POST['name'], time() + 3600); // Expires in 1 hour
}
?>
<!DOCTYPE html>
<html>
	<head>
        <title>My Python Form</title>
		<link rel="stylesheet" href="style.css">
	</head>
	<body class="php-body">
		<?php
			if (isset($_POST['name']) && $_POST['name'] !== '') { ?>
			<h1>Hello <?php echo $_POST['name']; ?> from PHP Script</h1>
		<?php
			}
			else if (isset($_COOKIE['myCookie'])) { ?>
			<h1>Hello <?php echo $_COOKIE['myCookie']; ?> from PHP Script</h1>
		<?php } ?>
    <form method="POST" action="/">
        <label for="name" class="php-button" >Enter a value:</label>
        <input type="text" name="name" id="name" class="php-button" >
        <button type="submit" class="php-button" >Submit</button>
    </form>
</body>
</html>
