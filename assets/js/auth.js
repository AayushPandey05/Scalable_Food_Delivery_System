let auth0 = null;

const configureClient = async () => {
    auth0 = await createAuth0Client({
        domain: "dev-tpfjrh1yyggihvc8.us.auth0.com",
        client_id: "nPhm2PIW29hUiaK2dHPjtDouPY9FOHtv",
        authorizationParams: {
            redirect_uri: window.location.origin
        }
    });
};

// This handles the redirect back from Auth0
const handleAuth = async () => {
    const query = window.location.search;
    if (query.includes("code=") && query.includes("state=")) {
        await auth0.handleRedirectCallback();
        window.history.replaceState({}, document.title, "/");
    }

    const isAuthenticated = await auth0.isAuthenticated();
    if (isAuthenticated) {
        const user = await auth0.getUser();
        // Update your UI here
        document.getElementById("login-btn").innerText = `Welcome, ${user.nickname} ✅`;
        console.log("User Identity Verified:", user);
    }
};

window.onload = async () => {
    await configureClient();
    await handleAuth();
};

// Function for your "Sign in with Okta/Auth0" button
const loginSSO = async () => {
    await auth0.loginWithRedirect();
};