const BACKEND_URL = "https://synthia-semidivine-therese.ngrok-free.dev";

async function handleAuth(event) {
  event.preventDefault();

  const isSignUp = !document
    .getElementById("group-name")
    .classList.contains("hidden");
  const email = document.getElementById("email").value;
  const password = document.getElementById("password").value;

  // If it's Sign Up, we use the name field. If Login, we just use a placeholder or skip.
  const username = isSignUp
    ? document.getElementById("fullName").value
    : email.split("@")[0];

  if (isSignUp) {
    await registerUser(username, email, password);
  } else {
    console.log("Login logic will go here next!");
    alert(
      "Login logic is coming in the next step, Aayush! Try Sign Up for now.",
    );
  }
}

async function registerUser(username, email, password) {
  try {
    const response = await fetch(`${BACKEND_URL}/api/register`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ username, email, password }),
    });

    const data = await response.json();

    if (response.ok && data.status === "success") {
      localStorage.setItem("authToken", data.token);
      alert(`Shabaash Aayush! User ${username} registered successfully.`);
      window.location.href = "index.html";
    } else {
      alert("Error: " + (data.message || "User already exists in AWS RDS"));
    }
  } catch (error) {
    console.error("Connection Error:", error);
    alert("checking if ngrok is running!");
  }
}
