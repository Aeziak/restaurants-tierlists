# NextAuth.js Google OAuth Setup Guide

## Prerequisites

- Google Cloud Platform account
- Project created in Google Cloud Console

## Step-by-Step Setup

### 1. Create OAuth 2.0 Credentials

1. Go to [Google Cloud Console](https://console.cloud.google.com/)
2. Select your project or create a new one
3. Navigate to **APIs & Services** > **Credentials**
4. Click **+ CREATE CREDENTIALS** > **OAuth client ID**
5. If prompted, configure the OAuth consent screen:
   - User Type: **External**
   - App name: **Restaurant Tier List**
   - User support email: Your email
   - Developer contact: Your email
   - Scopes: Add `email` and `profile`
   - Test users: Add your Google account email

### 2. Configure OAuth Client

1. Application type: **Web application**
2. Name: **Restaurant Tier List - Dev**
3. Authorized JavaScript origins:
   - `http://localhost:3000`
4. Authorized redirect URIs:
   - `http://localhost:3000/api/auth/callback/google`
5. Click **CREATE**
6. Copy the **Client ID** and **Client secret**

### 3. Setup Environment Variables

1. Copy `.env.local.example` to `.env.local`:
   ```bash
   cp .env.local.example .env.local
   ```

2. Edit `.env.local` and add your credentials:
   ```bash
   NEXTAUTH_URL=http://localhost:3000
   NEXTAUTH_SECRET=your-random-secret-here
   GOOGLE_CLIENT_ID=your-client-id-from-google
   GOOGLE_CLIENT_SECRET=your-client-secret-from-google
   NEXT_PUBLIC_API_URL=http://localhost:8080
   ```

3. Generate a secure `NEXTAUTH_SECRET`:
   ```bash
   openssl rand -base64 32
   ```

### 4. Test Authentication

1. Start the development server:
   ```bash
   pnpm dev
   ```

2. Open `http://localhost:3000`
3. Click **Sign In with Google**
4. Authorize the app with your Google account
5. You should be redirected back with your name and avatar displayed

### 5. Production Setup

For production deployment (Dokploy):

1. Create new OAuth credentials with production URLs:
   - Authorized JavaScript origins: `https://restaurant-tierlist.com`
   - Authorized redirect URIs: `https://restaurant-tierlist.com/api/auth/callback/google`

2. Add environment variables in Dokploy UI:
   ```bash
   NEXTAUTH_URL=https://restaurant-tierlist.com
   NEXTAUTH_SECRET=<production-secret>
   GOOGLE_CLIENT_ID=<production-client-id>
   GOOGLE_CLIENT_SECRET=<production-client-secret>
   ```

## Troubleshooting

### "Redirect URI mismatch" error

Make sure the redirect URI in Google Console exactly matches:
- Dev: `http://localhost:3000/api/auth/callback/google`
- Prod: `https://restaurant-tierlist.com/api/auth/callback/google`

### "Invalid client_id" error

Double-check that `GOOGLE_CLIENT_ID` in `.env.local` matches the Client ID from Google Console.

### Session not persisting

Ensure `NEXTAUTH_SECRET` is set and is a secure random string (at least 32 characters).

## References

- [NextAuth.js v5 Documentation](https://authjs.dev/)
- [Google OAuth 2.0 Setup](https://developers.google.com/identity/protocols/oauth2)
- [NextAuth Google Provider](https://authjs.dev/getting-started/providers/google)
