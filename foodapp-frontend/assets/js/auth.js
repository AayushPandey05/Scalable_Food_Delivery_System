let auth0 = null;

// 🛠️ SMART CONFIGURATION: Detects if you're on Localhost or GitHub
const configureClient = async () => {
  // This creates the full path (e.g., https://.../Scalable_Food_Delivery_System/)
  const currentPath = window.location.origin + window.location.pathname;

  auth0 = await createAuth0Client({
    domain: "dev-tpfjrh1yyggihvc8.us.auth0.com", // ⬅️ REPLACE with your actual Auth0 Domain
    client_id: "nPhm2PIW29hUiaK2dHPjtDouPY9FOHtv", // ⬅️ REPLACE with your Auth0 Client ID
    authorizationParams: {
      redirect_uri: currentPath,
    },
  });
};

// 🔐 HANDLE THE RETURN TRIP: This runs after you login with Google
const handleAuth = async () => {
  const query = window.location.search;

  // Check if URL has the 'code' from Auth0
  if (query.includes("code=") && query.includes("state=")) {
    try {
      await auth0.handleRedirectCallback();
      // Clean the URL (removes the ?code= part)
      window.history.replaceState({}, document.title, window.location.pathname);
      console.log("✅ Login Successful!");
    } catch (err) {
      console.error("❌ Error handling redirect:", err);
    }
  }

  const isAuthenticated = await auth0.isAuthenticated();

  if (isAuthenticated) {
    const user = await auth0.getUser();

    // 🛠️ UPDATE THE UI
    const loginBtn = document.getElementById("login-btn");
    if (loginBtn) {
      loginBtn.innerText = `Welcome, ${user.nickname || user.name} ✅`;
      loginBtn.style.backgroundColor = "#4CAF50"; // Turn button green on success
    }

    console.log("👤 User Identity Verified:", user);

    // FUTURE STEP: Send user to C++ Backend here
    // syncUserToBackend(user);
  }
};

// 🏁 INITIALIZE ON PAGE LOAD
window.onload = async () => {
  await configureClient();
  await handleAuth();
};

// 🚀 TRIGGER LOGIN: Call this from your "Sign in with Okta/Auth0" button
const loginSSO = async () => {
  try {
    console.log("Initiating Auth0 Redirect...");
    await auth0.loginWithRedirect();
  } catch (err) {
    console.error("❌ Login failed:", err);
  }
};
