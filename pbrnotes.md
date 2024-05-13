# PBR Notes

- metallic - all refracted light gets absorbed, no scatter.
- metal = opposite of dielectric
- refracted and reflected light are mutually exclusive.
- preserve energy conservation by: calculate specular fraction: % of incoming
  light's energy that is reflected. fraction of refracted light is 1 - reflected
  calculation
- with this approach, energy conserved since refracted/diffuse +
  reflected/specular can't exceed 1.0.

## Reflectance

- Reflectance equation: specific version of render equation
- Radiance: quantifies the magnitude of light coming from a single direction.
- Radiant flux φ: transmitted energy of a light source in Watts. Light is sum of
  energy over multiple wavelengths (function of wavelength). Radiant flux
  measures area of function of wavelengths, which is estimated by RGB/light
  color.
- Solid Angle w: size/area of a shape projected onto a unit sphere.
- Radiant intensity: amount of radiant flux per solid angle. IOW, the strength
  of a light source over a projected area onto the unit sphere. dφ/dw
- Radiance equation: total observed energy in area A over solid angle w of a
  light of radiant instensity φ. L = d^2\*φ/(dA\*dw\*cos(θ)). It's a radiometric
  measure of the amount of light in an area, scaled by the incident angle θ to
  the surface normal as cos(θ).
- If we consider solid angle w and area A to be infinitely small, use radiance
  to measure flux of a single ray of light hitting a single point in space.
  Translate solid angle w into a direction vector w, and A into a point p.
  Allows us to use radiance to calculate a single light ray's per fragment
  contribution.
- Irradiance: All incoming light onto a point p, the sum of radiance
- Lo(p,ωo)=∫Ω(fr(p,ωi,ωo)Li(p,ωi)n⋅ωidωi)
- L is radiance of point p and infinitely small solid angle wi, which can be
  though of as an incoming direction vector. cos(θ) scales the energy based on
  light's incident angle, which is calculated by n\*wi.

### BRDF

- BRDF: bidirectional reflective distribution function. Input: incoming light
  direction wi, outgoing view direction wo, surface normal n, and surface
  parameter a that represents microsurface's roughness. It approximates how much
  each individual light ray wi contributes to the final reflected light of an
  opaque surface given its material properties. For perfectly smooth surfaces,
  BRDF would return 0 for all light rays wi except the one ray that has the same
  reflected angle as the outgoing ray wo, where the function would return 1.
- BRDF aproximates material's reflective and refractive properties based on the
  microfacet theory. Blinn phong is a non energy conserving BRDF since it takes
  wi and wo as inputs. Most real time PBR use Cook-Torrance
- CT BRDF contains diffuse and specular part: fr= kd*flambert + ks* fct.
- kd is the ratio of incoming light that gets refracted (diffuse), and ks is
  ratio that is reflected. flambert is known as Lambertian diffuse similar to
  diffuse shading, = c/π. c is the albedo, and division by π normalizes the
  diffuse light as the earlier integral that contains the BRDF is scaled by π
  (coming in IBL).
- fct = DFG/(4(wo*n)(wi*n)). cook-torrance specular BRDF composed of 3 functions
  and a normalization factor in the denominator. 3 functions: normal
  Distribution function, Fresnel equation, and Geometry function.
- normal distribution function: approximates the amount of surface's microfacets
  that are aligned to the halfway vector, influenced by roughness of surface.
  This is the primary function approximating the microfacets. LearnOpenGL uses
  Trowbridge-Reitz GGX.
- Geometry function: describes self-shadowing property of the microfacets. When
  surface is rough, microfacet's can overshadow other microfacets reducing the
  light the surface reflects.
- Fresnel equation: describes ratio of surface reflection at difference surface
  angles. The moment light hits a surface, based on the surface-to-view angle,
  Fresnel equation tells us the % of light that gets reflected. more
  reflectivity closer to 90 degree angle. Theoretically, all surfaces fully
  reflect light if seen from perfect 90 degree angles. Fresnel-Schlick
  approximation is used to approximate Fresnel equation. Fresnel-Schlick only
  really defined for dielectric surfaces, since for conductor surfaces, base
  reflectivity with indices of refraction doesn't hold and needs a diff Fresnel
  equation. Hence, mix base reflectivity F0
