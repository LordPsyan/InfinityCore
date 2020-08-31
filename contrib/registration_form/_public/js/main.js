$(document).ready(function() {
    $("#stats a i").click(function() {
        $("#stats-box").slideToggle();
    });

    $(document).delegate("#form-register", "submit", function(e) {
        e.preventDefault();
        var username = $(this).find("input[name=username]").val();
        var password = $(this).find("input[name=password]").val();
        var rpassword = $(this).find("input[name=repeatpassword]").val();
        var email = $(this).find("input[name=email]").val();
        var expansion = $(this).find("input[name=expansion]").val();
		var token_key = $(this).find("input[name=token_key]").val();

        if(!username || !password || !rpassword || !email || !expansion || !token_key)
        {
            $("#message").addClass("error");
            $("#message").text("Fill in the empty fields.");
            return false;
        }
		
		if (token_key.length != 6 )
		{
			$("#message").addClass("error");
            $("#message").text("Pin must be 6 digits");
            return false;
		}
		
		if (isNaN(token_key))
		{
			$("#message").addClass("error");
            $("#message").text("Pin must be 0-9");
            return false;
		}

        if(password != rpassword)
        {
            $("#message").addClass("error");
            $("#message").text("Passwords don't match.");
            return false;
        }

        $.ajax({
            url: "ajax/register.php",
            type: "post",
            data: {username: username, password: password, email: email, expansion: expansion, token_key: token_key},
            beforeSend: function() {
                $("#form-register").find("input[type=submit]").val("Wait ...");
            }
        }).success(function(response) {
            if(response == 1) {
                $("#message").addClass("success");
                $("#message").text("Registration successful!");
            } else if(response == 2)
			{
				$("#message").addClass("error");
                $("#message").text("Username already taken");
			}
			else if(response == 0)
			{
				$("#message").addClass("error");
                $("#message").text("Please input all information");
			}
			else
			{
				$("#message").addClass("error");
                $("#message").text(response);
			}
            $("#form-register").find("input[type=submit]").val("Register");
        });
    });
});
