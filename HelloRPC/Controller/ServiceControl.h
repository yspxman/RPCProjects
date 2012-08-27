#pragma once

/// <summary>
///     Check if the specified service is running and if not start it.
/// </summary>
/// <param name="pszServiceName">Service short name.</param>
/// <returns>
///     A standard HRESULT to indicate pass/fail.
/// </returns>
HRESULT StartSvc ( _In_ PCWSTR pszServiceName );

/// <summary>
///     Stop the specified service 
/// </summary>
/// <param name="pszServiceName">Service short name.</param>
/// <returns>
///     A standard HRESULT to indicate pass/fail.
/// </returns>
HRESULT  StopSvc (_In_ PCWSTR pszServiceName );
